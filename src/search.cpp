#include "board.cpp"

// History
void update_history(i16& entry, int bonus) {
    entry += bonus - entry * abs(bonus) / HIST_MAX;
}

// Shared states
struct TTEntry {
    u16 hash;
    u16 move;
    i16 score;
    u8 depth;
    u8 bound;
};

TTEntry* TTABLE;
int RUNNING;
u64 LIMIT_SOFT;
u64 LIMIT_HARD;
u16 BEST_MOVE;

// Search stack
struct Stack {
    int eval = INF;
    u16 move = MOVE_NONE;
    u16 killer = MOVE_NONE;
};

// Search thread
struct Thread {
    u64 nodes;
    u16 pv;
    i16 qhist[4096];
    Stack stack[STACK_SIZE];
    vector<u64> visited;

    int search(Board& board, int alpha, int beta, int ply, int depth, int is_pv) {
        // Check qsearch
        int is_qsearch = depth <= 0;

        // Abort
        if (!(++nodes & 4095) && now() > LIMIT_HARD)
            RUNNING = FALSE;

        if (!RUNNING || ply >= MAX_PLY)
            return DRAW;

        // Oracle
        if (ply) {
            // Draw
            if (board.drawn(visited, ply))
                return DRAW;

            // Mate distance pruning
            alpha = max(alpha, ply - INF);
            beta = min(beta, INF - ply - 1);

            if (alpha >= beta)
                return alpha;
        }

        // Probe transposition table
        TTEntry& slot = TTABLE[board.hash >> (64 - TT_BITS)];
        TTEntry tt {};

        if (slot.hash == u16(board.hash)) {
            tt = slot;

            if (!is_pv && depth <= tt.depth && tt.bound != tt.score < beta)
                return tt.score;
        }

        // Killer
        stack[ply + 1].killer = MOVE_NONE;

        // Static eval
        int eval;

        stack[ply].eval = INF;

        // Best score
        int best = -INF;
        u16 best_move = MOVE_NONE;
        u8 bound = BOUND_UPPER;
        
        // Pruning
        if (!board.is_checked) {
            // Get eval
            eval = stack[ply].eval = board.eval();

            if (is_qsearch) {
                // Standpat
                best = eval;

                if (best >= beta)
                    return best;

                if (alpha < best)
                    alpha = best;
            }
            else if (!is_pv) {
                // Reverse futility pruning
                if (depth < 9 && eval < WIN && eval > beta + 70 * depth)
                    return eval;

                // Null move pruning
                if (depth > 2 && eval >= beta && stack[ply - 1].move && board.colors[board.stm] & ~board.pieces[PAWN] & ~board.pieces[KING]) {
                    int reduction = 5 + depth / 3;

                    Board child = board;

                    child.stm ^= 1;
                    child.hash ^= KEYS[PIECE_NONE][0];
                    child.enpassant = SQUARE_NONE;

                    stack[ply].move = MOVE_NONE;

                    int score = -search(child, -beta, -beta + 1, ply + 1, depth - reduction, FALSE);

                    if (score >= beta) {
                        return score < WIN ? score : beta;
                    }
                }
            }
        }

        // Generate move
        u16 move_list[MAX_MOVE];
        int move_scores[MAX_MOVE];
        int move_count = board.movegen(move_list, !is_qsearch || board.is_checked);

        // Score move
        for (int i = 0; i < move_count; i++) {
            int victim = board.board[move_to(move_list[i])] / 2;

            if (victim == TYPE_NONE)
                victim = PAWN;

            // Hash move
            if (move_list[i] == tt.move)
                move_scores[i] = 1e8;
            // Quiet moves
            else if (board.quiet(move_list[i]))
                move_scores[i] = move_list[i] == stack[ply].killer ? 1e6 : qhist[move_list[i] & 4095];
            // Noisy moves
            else
                move_scores[i] = PIECE_VALUE[victim] * 16 - PIECE_VALUE[board.board[move_from(move_list[i])] / 2] * 8 + 1e7;
        }

        // Iterate moves
        u16 quiet_list[MAX_MOVE];
        int quiet_count = 0;
        
        int legals = 0;

        for (int i = 0; i < move_count; i++) {
            // Sort next move
            int next_index = i;

            for (int k = i + 1; k < move_count; k++)
                if (move_scores[k] > move_scores[next_index])
                    next_index = k;
            
            swap(move_list[i], move_list[next_index]);
            swap(move_scores[i], move_scores[next_index]);

            u16 move = move_list[i];

            // Check if quiet
            int is_quiet = board.quiet(move);

            // Make
            Board child = board;

            if (!child.make(move))
                continue;

            stack[ply].move = move;
            visited.push_back(child.hash);

            legals++;

            // Search
            int score;

            // Don't do null window search for qsearch
            if (is_qsearch)
                goto pvsearch;

            // Null window search
            if (!is_pv || legals > 1)
                score = -search(child, -alpha - 1, -alpha, ply + 1, depth - 1, FALSE);

            // Principle variation search
            if (is_pv && (legals == 1 || score > alpha)) {
                pvsearch:
                score = -search(child, -beta, -alpha, ply + 1, depth - 1, is_qsearch ? is_pv : TRUE);
            }

            // Unmake
            visited.pop_back();

            // Abort
            if (!RUNNING)
                return DRAW;

            // Update score
            if (score > best)
                best = score;

            // Alpha raised
            if (score > alpha) {
                alpha = score;
                best_move = move;

                // Set exact bound
                bound = BOUND_EXACT;

                // Update pv
                if (!ply)
                    pv = move;
            }

            // Cutoff
            if (score >= beta) {
                // Set lower bound
                bound = BOUND_LOWER;

                // Skip for qsearch
                if (is_qsearch)
                    break;

                // History bonus
                int bonus = min(150 * depth - 50, 1500);

                if (is_quiet) {
                    // Killer
                    stack[ply].killer = move;

                    // Update quiet history
                    update_history(qhist[move & 4095], bonus);

                    for (int k = 0; k < quiet_count; k++)
                        update_history(qhist[quiet_list[k] & 4095], -bonus);
                }

                break;
            }

            // Push visited moves
            if (is_quiet)
                quiet_list[quiet_count++] = move;
        }

        // Return mate score
        if (!legals) {
            if (board.is_checked) return ply - INF;
            if (!is_qsearch) return DRAW;
        }

        // Update transposition
        slot = { u16(board.hash), best_move, i16(best), u8(!is_qsearch * depth), bound };

        return best;
    }

#ifdef OB
    void start(Board board, vector<u64>& pre_visited, int MAX_DEPTH = 256, int BENCH = FALSE) {
#else
    void start(Board board, vector<u64>& pre_visited) {
        #define MAX_DEPTH 256
#endif
        // Set data
        nodes = 0;
        pv = MOVE_NONE;
        visited = pre_visited;
        memset(qhist, 0, sizeof(qhist));

        int score = 0;

        // Iterative deepening
        for (int depth = 1; depth < MAX_DEPTH; ++depth) {
            // Clear stack
            for (Stack& ss : stack) ss = Stack();

            // Aspiration window
            int delta = 25;
            int alpha = score;
            int beta = score;

            while (score <= alpha || score >= beta) {
                // Update window
                alpha = score <= alpha ? score - delta : alpha;
                beta = score >= beta ? score + delta : beta;

                // Search
                score = search(board, alpha, beta, 0, depth, TRUE);

                // Scale delta
                delta *= 1.5;
            }

            // Print info
#ifdef OB
            if (!BENCH) {
#endif
                cout << "info depth " << depth << " score cp " << score << " pv ";
                move_print(pv);
#ifdef OB
            }
#endif

            // Check time
            if (now() > LIMIT_SOFT)
                RUNNING = FALSE;

            if (!RUNNING)
                break;
        }

        // Return best move
        BEST_MOVE = pv;
    }
};