#include "board.cpp"

// History
using HTable = i16[12][64];

void update_history(i16& entry, i32 bonus) {
    entry += bonus - entry * abs(bonus) / HIST_MAX;
}

// Search thread
struct Thread {
    i16 qhist[2][4096],
        corrhist[2][CORRHIST_SIZE];
    HTable nhist[6],
        conthist[12][64],
        *stack_conthist[STACK_SIZE];
    u64 nodes,
        nodes_table[4096],
        visited[STACK_SIZE];
    i32 id,
        stack_eval[STACK_SIZE];

    i32 search(Board& board, i32 alpha, i32 beta, i32 ply, i32 depth, i32 is_pv = FALSE, i32 excluded = MOVE_NONE) {
        // All search variables
        i32 eval,
            best = -INF,
            is_improving = FALSE,
            quiet_count = 0,
            noisy_count = 0,
            legals = 0,
            score,
            move_scores[MAX_MOVE];

        i16 move_list[MAX_MOVE],
            quiet_list[MAX_MOVE],
            noisy_list[MAX_MOVE];

        u8 bound = BOUND_UPPER;

        // Clamp depth for qsearch
        if (depth < 0)
            depth = 0;

        // Abort
        if (!id && !(++nodes & 4095) && now() > TIME_LIMIT)
            STOP++;

        if (STOP || ply >= MAX_PLY)
            return DRAW;

        // Oracle
        if (ply) {
            // Repetition
            for (i32 i = 4; i <= ply; i += 2)
                if (board.hash == visited[ply - i])
                    return DRAW;

            for (i32 i = 0; i < VISITED_COUNT; i++)
                if (board.hash == VISITED[i])
                    return DRAW;

            // Fifty-move rule
            if (board.halfmove > 99)
                return DRAW;
        }

        // Update hash history
        visited[ply] = board.hash;

        // Probe transposition table
        TTEntry tt = TTABLE[board.hash >> TT_SHIFT];

        if (tt.key != i16(board.hash))
            tt = {};

        // Cutoff
        else if (!is_pv && !excluded && depth <= tt.depth && tt.bound != tt.score < beta)
            return tt.score;

        // Static eval
        stack_eval[ply] = INF;
        
        // Pruning
        if (!board.checkers) {
            // Get eval
            eval = stack_eval[ply] = board.eval() +
                // Pawn corrhist
                corrhist[board.stm][board.hash_pawn % CORRHIST_SIZE] / 137 +
                // Non-pawn corrhist
                corrhist[board.stm][board.hash_non_pawn[WHITE] % CORRHIST_SIZE] / 208 +
                corrhist[board.stm][board.hash_non_pawn[BLACK] % CORRHIST_SIZE] / 208 +
                // Contcorrhist 1-ply
                stack_conthist[ply + 1][0][0][0] / 140 +
                // Contcorrhist 2-ply
                stack_conthist[ply][0][1][0] / 205;

            // Use tt score as better eval
            if (tt.key && !excluded && tt.bound != tt.score < eval)
                eval = tt.score;

            // Improving
            is_improving = ply > 1 && stack_eval[ply] > stack_eval[ply - 2];

            // Standpat
            if (!depth && (alpha = max(alpha, best = eval)) >= beta)
                return eval;

            // Reverse futility pruning
            if (!is_pv && !excluded && depth && depth < 9 && eval < WIN && eval > beta + 66 * depth - 66 * is_improving)
                return eval;

            // Null move pruning
            if (!is_pv && !excluded && depth > 2 && eval > beta + 25 && board.colors[board.stm] & ~board.pieces[PAWN] & ~board.pieces[KING]) {
                Board child = board;

                child.stm ^= 1;
                child.hash ^= KEYS[PIECE_NONE][0];
                child.hash ^= KEYS[PIECE_NONE][child.enpassant];
                child.enpassant = SQUARE_NONE;

                stack_conthist[ply + 2] = conthist[WHITE_PAWN];

                score = -search(child, -beta, -alpha, ply + 1, depth - 5 - depth / 3);

                if (score >= beta)
                    return score < WIN ? score : beta;
            }
        }

        // Generate moves
        i32 move_count = board.movegen(move_list, depth || board.checkers);

        // Score moves
        for (i32 i = 0; i < move_count; i++) {
            i32 move = move_list[i];

            move_scores[i] =
                // Hash move
                move == tt.move ? 1e8 :
                // Quiet moves
                board.quiet(move) ?
                    // Quiet history
                    qhist[board.stm][move & 4095] +
                    // Conthist 2-ply
                    2 * stack_conthist[ply][0][board.board[move_from(move)]][move_to(move)] +
                    // Conthist 1-ply
                    2 * stack_conthist[ply + 1][0][board.board[move_from(move)]][move_to(move)] :
                // Noisy moves
                    // MVV
                    VALUE[board.board[move_to(move)] / 2 % TYPE_NONE] * 16 +
                    // Noisy history
                    nhist[board.board[move_to(move)] / 2 % TYPE_NONE][board.board[move_from(move)]][move_to(move)] +
                    // SEE
                    board.see(move, 0) * 2e7 - 1e7;
        }

        // Iterate moves
        for (i32 i = 0; i < move_count; i++) {
            // Sort next move
            i32 next_index = i;

            for (i32 k = i + 1; k < move_count; k++)
                if (move_scores[k] > move_scores[next_index])
                    next_index = k;
            
            swap(move_list[i], move_list[next_index]);
            swap(move_scores[i], move_scores[next_index]);

            // Search data
            i32 move = move_list[i],
                is_quiet = board.quiet(move),
                depth_next = depth - 1;

            // Start nodes count
            u64 nodes_start = nodes;

            // Skip excluded move in singularity search
            if (move == excluded)
                continue;

            // Quiet pruning and SEE pruning in qsearch
            if (!depth && best > -WIN && move_scores[i] < 1e6)
                break;

            // Late move pruning
            if (ply && best > -WIN && quiet_count > 1 + depth * depth >> !is_improving && is_quiet)
                continue;

            // Futility pruning
            if (ply && best > -WIN && depth < 10 && !board.checkers && stack_eval[ply] + 96 * depth + move_scores[i] / 32 + 100 < alpha && is_quiet)
                continue;

            // SEE pruning in pvsearch
            if (ply && best > -WIN && move_scores[i] < 1e6 && !board.see(move, -81 * depth))
                continue;

            // Singular extension
            if (ply && depth > 7 && !excluded && move == tt.move && tt.depth > depth - 4 && tt.bound && abs(tt.score) < WIN) {
                i32 singular_beta = tt.score - depth;
                score = search(board, singular_beta - 1, singular_beta, ply, (depth - 1) / 2, FALSE, move);

                // Single extension + double extension
                if (score < singular_beta)
                    depth_next += 1 + (!is_pv && score + 13 < singular_beta);

                // Multicut
                else if (score >= beta)
                    return score;
            }

            // Make
            Board child = board;

            if (child.make(move))
                continue;

            stack_conthist[ply + 2] = &conthist[board.board[move_from(move)]][move_to(move)];

            // Set this as a dummy value to drop straight into ZWS if we don't do LMR
            score = beta;

            // Late move reduction
            if (depth > 2 && legals > 2) {
                i32 reduction =
                    // Base reduction
                    log(depth) * log(legals + 1) * 0.35 + 1 -
                    // History reduction
                    is_quiet * move_scores[i] / 7792 +
                    // PV
                    !is_pv;

                // Clamp noisy reduction
                if (!is_quiet && reduction > 2)
                    reduction = 2;

                if (reduction > 0)
                    score = -search(child, -alpha - 1, -alpha, ply + 1, depth_next - reduction);
            }

            // Zero window search (don't do it for qsearch)
            if (score > alpha && depth && legals)
                score = -search(child, -alpha - 1, -alpha, ply + 1, depth_next);

            // Principal variation search and qsearch
            if (!depth || !legals || is_pv && score > alpha)
                score = -search(child, -beta, -alpha, ply + 1, depth_next, is_pv);

            legals++;

            // Abort
            if (STOP)
                return DRAW;

            // Update root moves nodes count
            if (!ply)
                nodes_table[move & 4095] += nodes - nodes_start;

            // Update score
            if (score > best)
                best = score;

            // Alpha raised
            if (score > alpha) {
                alpha = score;
                tt.move = move;

                // Set exact bound
                bound = BOUND_EXACT;

                // Update pv
                if (!id && !ply)
                    BEST_MOVE = move;
            }

            // Cutoff
            if (score >= beta) {
                // Set lower bound
                bound = BOUND_LOWER;

                // Skip for qsearch
                if (!depth)
                    break;

                // History bonus
                i32 bonus = min(157 * depth - 54, 1485) + (stack_eval[ply] <= best) * 150;

                if (is_quiet) {
                    // Update quiet history
                    update_history(qhist[board.stm][move & 4095], bonus);
                    update_history(stack_conthist[ply][0][board.board[move_from(move)]][move_to(move)], bonus);
                    update_history(stack_conthist[ply + 1][0][board.board[move_from(move)]][move_to(move)], bonus);

                    // Add penalty to visited quiet moves
                    for (i32 k = 0; k < quiet_count; k++)
                        update_history(qhist[board.stm][quiet_list[k] & 4095], -bonus),
                        update_history(stack_conthist[ply][0][board.board[move_from(quiet_list[k])]][move_to(quiet_list[k])], -bonus),
                        update_history(stack_conthist[ply + 1][0][board.board[move_from(quiet_list[k])]][move_to(quiet_list[k])], -bonus);
                }
                else
                    // Update noisy history
                    update_history(nhist[board.board[move_to(move)] / 2 % TYPE_NONE][board.board[move_from(move)]][move_to(move)], bonus);

                // Add penalty to visited noisy moves
                for (i32 k = 0; k < noisy_count; k++)
                    update_history(nhist[board.board[move_to(noisy_list[k])] / 2 % TYPE_NONE][board.board[move_from(noisy_list[k])]][move_to(noisy_list[k])], -bonus);

                break;
            }

            // Push visited moves
            (is_quiet ? quiet_list[quiet_count++] : noisy_list[noisy_count++]) = move;
        }

        // Return mate score
        if (!legals) {
            if (board.checkers) return ply - INF;
            if (depth) return DRAW;
        }

        // Update corrhist
        if (!board.checkers && (!bound || board.quiet(tt.move)) && bound != best < stack_eval[ply]) {
            i32 bonus = clamp((best - stack_eval[ply]) * depth, -CORRHIST_BONUS_MAX, CORRHIST_BONUS_MAX) * CORRHIST_BONUS_SCALE;

            update_history(corrhist[board.stm][board.hash_pawn % CORRHIST_SIZE], bonus);
            update_history(corrhist[board.stm][board.hash_non_pawn[WHITE] % CORRHIST_SIZE], bonus);
            update_history(corrhist[board.stm][board.hash_non_pawn[BLACK] % CORRHIST_SIZE], bonus);
            update_history(stack_conthist[ply + 1][0][0][0], bonus);
            update_history(stack_conthist[ply][0][1][0], bonus);
        }

        // Update transposition
        if (!excluded)
            TTABLE[board.hash >> TT_SHIFT] = { i16(board.hash), tt.move, i16(best), u8(depth), bound };

        return best;
    }

#ifdef OB_MINI
    void start(Board board, i32 ID, i32 MAX_DEPTH = 256, i32 BENCH = FALSE) {
#else
    void start(Board board, i32 ID) {
        #define MAX_DEPTH 256
#endif
        id = ID;
        i32 score;

        // Iterative deepening
        for (i32 depth = 1; depth < MAX_DEPTH; depth++) {
            // Clear stack
            stack_conthist[0] = stack_conthist[1] = &conthist[WHITE_PAWN][B1];

            // Aspiration window
            i32 delta = 10,
                alpha = depth > 3 ? score - delta : -INF,
                beta = depth > 3 ? score + delta : INF,
                reduction = 0;

            for (;;) {
                // Search
                score = search(board, alpha, beta, 0, max(depth - reduction, 1), TRUE);

                // Update window
                if (score <= alpha)
                    beta = (alpha + beta) / 2,
                    alpha = score - delta,
                    reduction = 0;
                else if (score >= beta)
                    beta = score + delta,
                    reduction++;
                else
                    break;

                // Scale delta
                delta *= 1.5;
            }

            // Print info
#ifdef OB_MINI
            if (!id && !BENCH)
#else
            if (!id)
#endif
                cout << "info depth " << depth << " score cp " << score << " pv ", move_print(BEST_MOVE);

            // Check time
            if (!id && now() > TIME_START + TIME_SOFT * (2 - 1.5 * nodes_table[BEST_MOVE & 4095] / nodes))
                STOP++;

            if (STOP)
                break;
        }

        // Return best move
#ifdef OB_MINI
        if (!id && !BENCH)
#else
        if (!id)
#endif
            STOP++,
            cout << "bestmove ", move_print(BEST_MOVE);
    }
};