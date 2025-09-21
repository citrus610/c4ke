#include "board.cpp"

// History
typedef i16 HTable[12][64];

void update_history(i16& entry, int bonus) {
    entry += bonus - entry * abs(bonus) / HIST_MAX;
}

// Search thread
struct Thread {
    u16 pv;
    i16 qhist[2][4096] {},
        corrhist[2][CORRHIST_SIZE] {};
    HTable nhist[6] {},
        conthist[12][64] {},
        *stack_conthist[STACK_SIZE];
    u64 nodes {},
        visited[STACK_SIZE];
    int id,
        stack_eval[STACK_SIZE];

    int search(Board& board, int alpha, int beta, int ply, int depth, int is_pv = FALSE, u16 excluded = MOVE_NONE) {
        // All search variables
        int eval,
            best = -INF,
            is_improving = FALSE,
            quiet_count = 0,
            noisy_count = 0,
            legals = 0,
            move_scores[MAX_MOVE];

        u16 best_move = MOVE_NONE,
            move_list[MAX_MOVE],
            quiet_list[MAX_MOVE],
            noisy_list[MAX_MOVE];

        u8 bound = BOUND_UPPER;

        // Clamp depth for qsearch
        depth *= depth > 0;

        // Abort
        if (!(++nodes & 4095) && now() > LIMIT_HARD && !id)
            RUNNING = FALSE;

        if (!RUNNING || ply >= MAX_PLY)
            return DRAW;

        // Oracle
        if (ply) {
            // Repetiion
            for (int i = 4; i <= ply; i += 2)
                if (board.hash == visited[ply - i])
                    return DRAW;

            for (int i = 0; i < VISITED_COUNT; i++)
                if (board.hash == VISITED[i])
                    return DRAW;

            // Draw by 50mr
            if (board.halfmove > 99)
                return DRAW;
        }

        // Update hash history
        visited[ply] = board.hash;

        // Probe transposition table
        TTEntry& slot = TTABLE[board.hash >> TT_SHIFT];
        TTEntry tt {};

        if (slot.key == u16(board.hash)) {
            tt = slot;

            // Cutoff
            if (!is_pv && !excluded && depth <= tt.depth && tt.bound != tt.score < beta)
                return tt.score;
        }
        else
            // Internal iterative reduction
            depth -= is_pv && depth > 3;

        // Static eval
        stack_eval[ply] = INF;
        
        // Pruning
        if (!board.checkers) {
            // Get eval
            eval = stack_eval[ply] = board.eval() +
                corrhist[board.stm][board.hash_pawn % CORRHIST_SIZE] / 128 +
                corrhist[board.stm][board.hash_non_pawn[WHITE] % CORRHIST_SIZE] / 256 +
                corrhist[board.stm][board.hash_non_pawn[BLACK] % CORRHIST_SIZE] / 256 +
                (*stack_conthist[ply + 1])[0][0] / 128 +
                (*stack_conthist[ply])[1][0] / 200;

            // Use tt score as better eval
            if (tt.key && !excluded && tt.bound != tt.score < eval)
                eval = tt.score;

            // Improving
            is_improving = ply > 1 && stack_eval[ply] > stack_eval[ply - 2];

            if (!depth) {
                // Standpat
                if (eval >= beta)
                    return eval;

                alpha = max(alpha, best = eval);
            }
            else if (!is_pv && !excluded) {
                // Reverse futility pruning
                if (depth < 9 && eval < WIN && eval > beta + 70 * (depth - is_improving))
                    return eval;

                // Null move pruning
                if (depth > 2 && eval >= beta && board.colors[board.stm] & ~board.pieces[PAWN] & ~board.pieces[KING]) {
                    Board child = board;

                    if (child.enpassant < SQUARE_NONE)
                        child.hash ^= KEYS[PIECE_NONE][child.enpassant];

                    child.stm ^= 1;
                    child.hash ^= KEYS[PIECE_NONE][0];
                    child.enpassant = SQUARE_NONE;

                    stack_conthist[ply + 2] = conthist[WHITE_PAWN];

                    int score = -search(child, -beta, -alpha, ply + 1, depth - 5 - depth / 3);

                    if (score >= beta)
                        return score < WIN ? score : beta;
                }
            }
        }

        // Generate move
        int move_count = board.movegen(move_list, depth || board.checkers);

        // Score move
        for (int i = 0; i < move_count; i++) {
            int move = move_list[i],
                piece = board.board[move_from(move)],
                victim = board.board[move_to(move)] / 2 % TYPE_NONE;

            move_scores[i] =
                // Hash move
                move == tt.move ? 1e8 :
                // Quiet moves
                board.quiet(move) ? qhist[board.stm][move & 4095] + (*stack_conthist[ply])[piece][move_to(move)] + (*stack_conthist[ply + 1])[piece][move_to(move)] :
                // Noisy moves
                VALUE[victim] * 16 + VALUE[move_promo(move)] + nhist[victim][piece][move_to(move)] + board.see(move, 0) * 2e7 - 1e7;
        }

        // Iterate moves
        for (int i = 0; i < move_count; i++) {
            // Sort next move
            int next_index = i;

            for (int k = i + 1; k < move_count; k++)
                if (move_scores[k] > move_scores[next_index])
                    next_index = k;
            
            swap(move_list[i], move_list[next_index]);
            swap(move_scores[i], move_scores[next_index]);

            u16 move = move_list[i];

            // Skip excluded move in singularity search
            if (move == excluded)
                continue;

            // Search data
            int is_quiet = board.quiet(move),
                depth_next = depth - 1,
                score;

            // Quiet pruning and SEE pruning in qsearch
            if (!depth && best > -WIN && move_scores[i] < 1e6)
                break;

            // Late move pruning
            if (!is_pv && !board.checkers && quiet_count > 1 + depth * depth >> !is_improving && is_quiet)
                continue;

            // Futility pruning
            if (ply && best > -WIN && depth < 10 && !board.checkers && stack_eval[ply] + 100 * depth + 100 < alpha && is_quiet)
                continue;

            // SEE pruning
            if (ply && best > -WIN && move_scores[i] < 1e6 && !board.see(move, -80 * depth))
                continue;

            // Singular extension
            if (ply && depth > 7 && !excluded && move == tt.move && tt.depth > depth - 4 && tt.bound) {
                int singular_beta = tt.score - depth * 2,
                    singular_score = search(board, singular_beta - 1, singular_beta, ply, (depth - 1) / 2, FALSE, move);

                if (singular_score < singular_beta)
                    depth_next += 1 + (!is_pv && singular_score + 16 < singular_beta);
                else if (singular_score >= beta)
                    return singular_score;
            }

            // Make
            Board child = board;

            if (child.make(move))
                continue;

            legals++;

            stack_conthist[ply + 2] = &conthist[board.board[move_from(move)]][move_to(move)];

            // Don't do zero window search for qsearch
            // Late move reduction
            if (depth > 2 && legals > 3) {
                int reduction =
                    // Base reduction
                    log(depth) * log(legals) * 0.3 + 1 -
                    // History reduction
                    is_quiet * move_scores[i] / 8192 +
                    // PV
                    !is_pv;

                reduction *= reduction > 0;

                score = -search(child, -alpha - 1, -alpha, ply + 1, depth_next - reduction);

                if (score > alpha && reduction)
                    score = -search(child, -alpha - 1, -alpha, ply + 1, depth_next);
            }
            // Zero window search
            else if (depth && (!is_pv || legals > 1))
                score = -search(child, -alpha - 1, -alpha, ply + 1, depth_next);

            // Principle variation search and qsearch
            if (!depth || (is_pv && (legals == 1 || score > alpha)))
                score = -search(child, -beta, -alpha, ply + 1, depth_next, is_pv);

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
                if (!depth)
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

                // Add penalty to visited noisy moves
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
            if (board.checkers) return ply - INF;
            if (depth) return DRAW;
        }

        // Update corrhist
        if (!board.checkers && (!best_move || board.quiet(best_move)) && bound != best < stack_eval[ply]) {
            int bonus = clamp((best - stack_eval[ply]) * depth, -CORRHIST_BONUS_MAX, CORRHIST_BONUS_MAX) * CORRHIST_BONUS_SCALE;

            update_history(corrhist[board.stm][board.hash_pawn % CORRHIST_SIZE], bonus);
            update_history(corrhist[board.stm][board.hash_non_pawn[WHITE] % CORRHIST_SIZE], bonus);
            update_history(corrhist[board.stm][board.hash_non_pawn[BLACK] % CORRHIST_SIZE], bonus);
            update_history((*stack_conthist[ply + 1])[0][0], bonus);
            update_history((*stack_conthist[ply])[1][0], bonus);
        }

        // Update transposition
        if (!excluded)
            slot = { u16(board.hash), best_move || !tt.key ? best_move : slot.move, i16(best), u8(depth), bound };

        return best;
    }

#ifdef OB_MINI
    void start(Board board, int ID = 0, int MAX_DEPTH = 256, int BENCH = FALSE) {
#else
    void start(Board board, int ID) {
        #define MAX_DEPTH 256
#endif
        id = ID;

        // Iterative deepening
        for (int score = 0, depth = 1; depth < MAX_DEPTH; ++depth) {
            // Clear stack
            stack_conthist[0] = stack_conthist[1] = &conthist[WHITE_PAWN][B1];

            // Aspiration window, scale delta every failed iteration
            for (int delta = 10, alpha = score, beta = score; score <= alpha || score >= beta; delta *= 1.5) {
                // Update window
                if (score <= alpha) alpha = score - delta;
                if (score >= beta) beta = score + delta;

                // Search
                score = search(board, alpha, beta, 0, depth, TRUE);
            }

            // Print info
#ifdef OB_MINI
            if (!BENCH && !id) {
#endif
                cout << "info depth " << depth << " score cp " << score << " pv ";
                move_print(pv);
#ifdef OB_MINI
            }
#endif

            // Check time
            if (now() > LIMIT_SOFT && !id)
                RUNNING = FALSE;

            if (!RUNNING)
                break;
        }

        // Return best move
        if (!id)
            BEST_MOVE = pv;
    }

};
