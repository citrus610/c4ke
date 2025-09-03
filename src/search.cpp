#include "board.cpp"

// History
typedef i16 HTable[12][64];

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

// Search thread
struct Thread {
    u64 nodes {};
    u16 pv {};
    i16 qhist[2][4096] {};
    HTable nhist[6] {};
    HTable conthist[12][64] {};
    i16 corrhist[2][CORRHIST_SIZE] {};
    int stack_eval[STACK_SIZE];
    HTable* stack_conthist[STACK_SIZE];
    vector<u64> visited;

    int search(Board& board, int alpha, int beta, int ply, int depth, int is_pv, int is_nmp = FALSE) {
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
            int before_root = FALSE;

            for (int i = 4; i <= board.halfmove && i <= visited.size(); i += 2) {
                if (board.hash == visited[visited.size() - i]) {
                    if (ply >= i || before_root)
                        return DRAW;

                    before_root = TRUE;
                }
            }

            if (board.halfmove > 99)
                return DRAW;
        }

        // Probe transposition table
        TTEntry& slot = TTABLE[board.hash >> (64 - TT_BITS)];
        TTEntry tt {};

        if (slot.hash == u16(board.hash)) {
            tt = slot;

            // Cutoff
            if (!is_pv && depth <= tt.depth && tt.bound != tt.score < beta)
                return tt.score;
        }
        else
            // Internal iterative reduction
            depth -= depth > 3;

        // Static eval
        int eval;
        stack_eval[ply] = INF;

        // Best score
        int best = -INF;
        u16 best_move = MOVE_NONE;
        u8 bound = BOUND_UPPER;
        
        // Pruning
        if (!board.is_checked) {
            // Get eval
            eval = stack_eval[ply] = board.eval() +
                corrhist[board.stm][board.hash_pawn % CORRHIST_SIZE] / 128 +
                corrhist[board.stm][board.hash_non_pawn[WHITE] % CORRHIST_SIZE] / 256 +
                corrhist[board.stm][board.hash_non_pawn[BLACK] % CORRHIST_SIZE] / 256;

            // Use tt score as better eval
            if (tt.hash && tt.bound != tt.score < eval)
                eval = tt.score;

            if (is_qsearch) {
                // Standpat
                best = eval;

                if (best >= beta)
                    return best;

                alpha = max(alpha, best);
            }
            else if (!is_pv) {
                // Reverse futility pruning
                if (depth < 9 && eval < WIN && eval > beta + 70 * depth)
                    return eval;

                // Null move pruning
                if (depth > 2 && eval >= beta && !is_nmp && board.colors[board.stm] & ~board.pieces[PAWN] & ~board.pieces[KING]) {
                    int reduction = 5 + depth / 3;

                    Board child = board;

                    child.stm ^= 1;
                    child.hash ^= KEYS[PIECE_NONE][0];
                    child.enpassant = SQUARE_NONE;

                    stack_conthist[ply + 2] = conthist[WHITE_PAWN];

                    int score = -search(child, -beta, -beta + 1, ply + 1, depth - reduction, FALSE, TRUE);

                    if (score >= beta)
                        return score < WIN ? score : beta;
                }
            }
        }

        // Generate move
        u16 move_list[MAX_MOVE];
        int move_scores[MAX_MOVE];
        int move_count = board.movegen(move_list, !is_qsearch || board.is_checked);

        // Score move
        for (int i = 0; i < move_count; i++) {
            int move = move_list[i];
            int piece = board.board[move_from(move)];
            int victim = board.board[move_to(move)] / 2 % TYPE_NONE;

            move_scores[i] =
                // Hash move
                move == tt.move ? 1e7 :
                // Quiet moves
                board.quiet(move) ? qhist[board.stm][move & 4095] + (*stack_conthist[ply])[piece][move_to(move)] + (*stack_conthist[ply + 1])[piece][move_to(move)] :
                // Noisy moves
                PIECE_VALUE[victim] * 16 + PIECE_VALUE[move_promo(move)] + nhist[victim][piece][move_to(move)] + 1e6;
        }

        // Iterate moves
        u16 quiet_list[MAX_MOVE];
        u16 noisy_list[MAX_MOVE];
        int quiet_count = 0;
        int noisy_count = 0;
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

            // Quiet pruning in qsearch
            if (is_qsearch && best > -WIN && board.is_checked && is_quiet)
                break;

            // Delta pruning
            if (is_qsearch && !board.is_checked && !move_promo(move) && eval + 100 + PIECE_VALUE[board.board[move_to(move)] / 2] < alpha)
                continue;

            // Late move pruning
            if (!is_pv && !board.is_checked && quiet_count > depth * depth + 1)
                break;

            // Make
            Board child = board;

            if (!child.make(move))
                continue;

            legals++;

            stack_conthist[ply + 2] = &conthist[board.board[move_from(move)]][move_to(move)];

            visited.push_back(child.hash);

            // Search
            int depth_next = depth - 1;
            int score;

            // Don't do zero window search for qsearch
            if (is_qsearch)
                goto pvsearch;

            // Late move reduction
            if (depth > 2 && legals > 1 + !!ply * 2) {
                int reduction = log(depth) * log(legals) * 0.3 + 1;

                if (is_quiet)
                    reduction -= move_scores[i] / 8192;

                reduction *= reduction > 0;

                score = -search(child, -alpha - 1, -alpha, ply + 1, depth_next - reduction, FALSE);

                if (score > alpha && reduction)
                    goto zwsearch;
            }
            // Zero window search
            else if (!is_pv || legals > 1)
                zwsearch:
                score = -search(child, -alpha - 1, -alpha, ply + 1, depth_next, FALSE);

            // Principle variation search
            if (is_pv && (legals == 1 || score > alpha))
                pvsearch:
                score = -search(child, -beta, -alpha, ply + 1, depth_next, is_pv);

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
                    // Update quiet history
                    update_history(qhist[board.stm][move & 4095], bonus);
                    update_history((*stack_conthist[ply])[board.board[move_from(move)]][move_to(move)], bonus);
                    update_history((*stack_conthist[ply + 1])[board.board[move_from(move)]][move_to(move)], bonus);

                    // Add pelnaty to visited quiet moves
                    for (int k = 0; k < quiet_count; k++) {
                        update_history(qhist[board.stm][quiet_list[k] & 4095], -bonus);
                        update_history((*stack_conthist[ply])[board.board[move_from(quiet_list[k])]][move_to(quiet_list[k])], -bonus);
                        update_history((*stack_conthist[ply + 1])[board.board[move_from(quiet_list[k])]][move_to(quiet_list[k])], -bonus);
                    }
                }
                else
                    // Update noisy history
                    update_history(nhist[board.board[move_to(move)] / 2 % TYPE_NONE][board.board[move_from(move)]][move_to(move)], bonus);

                // Add pelnaty to visited noisy moves
                for (int k = 0; k < noisy_count; k++)
                    update_history(nhist[board.board[move_to(noisy_list[k])] / 2 % TYPE_NONE][board.board[move_from(noisy_list[k])]][move_to(noisy_list[k])], -bonus);

                break;
            }

            // Push visited moves
            if (is_quiet)
                quiet_list[quiet_count++] = move;
            else
                noisy_list[noisy_count++] = move;
        }

        // Return mate score
        if (!legals) {
            if (board.is_checked) return ply - INF;
            if (!is_qsearch) return DRAW;
        }

        // Update corrhist
        if (!board.is_checked && (!best_move || board.quiet(best_move)) && bound != best < stack_eval[ply]) {
            int bonus = clamp((best - stack_eval[ply]) * depth, -CORRHIST_BONUS_MAX, CORRHIST_BONUS_MAX) * CORRHIST_BONUS_SCALE;

            update_history(corrhist[board.stm][board.hash_pawn % CORRHIST_SIZE], bonus);
            update_history(corrhist[board.stm][board.hash_non_pawn[WHITE] % CORRHIST_SIZE], bonus);
            update_history(corrhist[board.stm][board.hash_non_pawn[BLACK] % CORRHIST_SIZE], bonus);
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
        visited = pre_visited;

        int score = 0;

        // Iterative deepening
        for (int depth = 1; depth < MAX_DEPTH; ++depth) {
            // Clear stack
            stack_conthist[0] = &conthist[WHITE_PAWN][B1];
            stack_conthist[1] = &conthist[WHITE_PAWN][B1];

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