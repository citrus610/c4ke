#include "eval.cpp"

int LAYOUT[] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };

void add_pawn_moves(u16 list[], int& count, u64 targets, int offset) {
    while (targets) {
        int to = LSB(targets);
        int from = to - offset;

        targets &= targets - 1;

        if (to < 8 || to > 55) {
            list[count++] = move_make(from, to, KNIGHT);
            list[count++] = move_make(from, to, BISHOP);
            list[count++] = move_make(from, to, ROOK);
            list[count++] = move_make(from, to, QUEEN);
        }
        else
            list[count++] = move_make(from, to);
    }
}

void add_moves(u16 list[], int& count, u64 mask, u64 targets, u64 occupied, u64 (*func)(u64, u64)) {
    while (mask) {
        int from = LSB(mask);
        mask &= mask - 1;

        u64 attack = func(1ull << from, occupied) & targets;

        while (attack) {
            int to = LSB(attack);
            attack &= attack - 1;

            list[count++] = move_make(from, to);
        }
    }
}

struct Board {
    u64 pieces[6];
    u64 colors[2];
    u8 board[64];
    int stm;
    int castled;
    int enpassant;
    int halfmove;
    u64 checkers;
    u64 hash;
    u64 hash_pawn;
    u64 hash_non_pawn[2];

    void edit(int square, int piece) {
        u64 mask = 1ull << square;

        if (board[square] < PIECE_NONE) {
            hash ^= KEYS[board[square]][square];

            pieces[board[square] / 2] &= ~mask;
            colors[board[square] & 1] &= ~mask;

            if (board[square] / 2 < KNIGHT)
                hash_pawn ^= KEYS[board[square]][square];
            else
                hash_non_pawn[board[square] & 1] ^= KEYS[board[square]][square];
        }

        if (piece < PIECE_NONE) {
            hash ^= KEYS[piece][square];

            pieces[piece / 2] |= mask;
            colors[piece & 1] |= mask;

            if (piece / 2 < KNIGHT)
                hash_pawn ^= KEYS[piece][square];
            else
                hash_non_pawn[piece & 1] ^= KEYS[piece][square];
        }

        board[square] = piece;
    }

    u64 attackers(int square) {
        return
            (nw(1ull << square) | ne(1ull << square)) & pieces[PAWN] & colors[BLACK] |
            (sw(1ull << square) | se(1ull << square)) & pieces[PAWN] & colors[WHITE] |
            knight(1ull << square) & pieces[KNIGHT] |
            bishop(1ull << square, colors[WHITE] | colors[BLACK]) & (pieces[BISHOP] | pieces[QUEEN]) |
            rook(1ull << square, colors[WHITE] | colors[BLACK]) & (pieces[ROOK] | pieces[QUEEN]) |
            king(1ull << square) & pieces[KING];
    }

    int quiet(u16 move) {
        return board[move_to(move)] > BLACK_KING && !move_promo(move) && !(board[move_from(move)] < WHITE_KNIGHT && move_to(move) == enpassant);
    }

    int see(u16 move, int threshold) {
        int from = move_from(move);
        int to = move_to(move);

        if (move_promo(move) || (board[from] < WHITE_KNIGHT && to == enpassant))
            return TRUE;

        if ((threshold -= VALUE[board[to] / 2]) > 0)
            return FALSE;

        if ((threshold += VALUE[board[from] / 2]) <= 0)
            return TRUE;

        u64 colors_original[] = { colors[WHITE], colors[BLACK] };
        colors[stm] ^= 1ull << from;
        int side = !stm;

        while (u64 threats = attackers(to) & colors[side]) {
            int type = PAWN;

            for (; type < KING; type++)
                if (pieces[type] & threats)
                    break;

            side ^= 1;

            if ((threshold = VALUE[type] - threshold) < 0) {
                side ^= type == KING && attackers(to) & colors[side];

                break;
            }

            colors[!side] ^= 1ull << LSB(pieces[type] & threats);
        }
        
        colors[WHITE] = colors_original[WHITE];
        colors[BLACK] = colors_original[BLACK];

        return side != stm;
    }

    u64 make(u16 move) {
        // Get move data
        int from = move_from(move);
        int to = move_to(move);
        int piece = board[from];

        // Update halfmove
        halfmove += board[to] > BLACK_KING && piece > BLACK_PAWN ? 1 : -halfmove;

        // Move piece
        edit(to, move_promo(move) ? move_promo(move) * 2 + stm : piece);
        edit(from, PIECE_NONE);

        // Enpassant
        hash ^= KEYS[PIECE_NONE][enpassant];

        if (piece < WHITE_KNIGHT && to == enpassant) {
            edit(to ^ 8, PIECE_NONE);
            hash ^= KEYS[PIECE_NONE][to];
        }

        enpassant = piece < WHITE_KNIGHT ? abs(from - to) == 16 ? to ^ 8 : SQUARE_NONE : SQUARE_NONE;

        hash ^= KEYS[PIECE_NONE][enpassant];

        // Castling
        hash ^= KEYS[PIECE_NONE][castled];

        if (piece > BLACK_QUEEN) {
            if (abs(from - to) == 2) {
                int dt = to > from ? 1 : -1;

                if ((attackers(from + dt) | attackers(from + dt * 2)) & colors[!stm])
                    return TRUE;

                edit(to + (to > from ? 1 : -2), PIECE_NONE);
                edit(from + dt, ROOK * 2 + stm);
            }

            castled |= 3 << stm * 2;
        }

        if (from == H1 || to == H1) castled |= CASTLED_WK;
        if (from == A1 || to == A1) castled |= CASTLED_WQ;
        if (from == H8 || to == H8) castled |= CASTLED_BK;
        if (from == A8 || to == A8) castled |= CASTLED_BQ;

        hash ^= KEYS[PIECE_NONE][castled];

        // Update side to move
        stm ^= 1;
        hash ^= KEYS[PIECE_NONE][0];

        // In check
        checkers = attackers(LSB(pieces[KING] & colors[stm])) & colors[!stm];

        // Check if not legal
        return attackers(LSB(pieces[KING] & colors[!stm])) & colors[stm];
    }

    int movegen(u16 list[], int is_all) {
        int count = 0;

        u64 occupied = colors[WHITE] | colors[BLACK];
        u64 targets = is_all ? ~colors[stm] : colors[!stm];

        // Pawn
        u64 pawns = pieces[PAWN] & colors[stm];
        u64 pawns_push = (stm ? south(pawns) : north(pawns)) & ~occupied & (is_all ? ~0ull : 0xff000000000000ffull);
        u64 pawns_targets = colors[!stm] | u64(enpassant < SQUARE_NONE) << enpassant;

        add_pawn_moves(list, count, pawns_push, stm ? -8 : 8);
        add_pawn_moves(list, count, (stm ? south(pawns_push & 0xff0000000000ull) : north(pawns_push & 0xff0000ull)) & ~occupied, stm ? -16 : 16);
        add_pawn_moves(list, count, (stm ? se(pawns) : nw(pawns)) & pawns_targets, stm ? -7 : 7);
        add_pawn_moves(list, count, (stm ? sw(pawns) : ne(pawns)) & pawns_targets, stm ? -9 : 9);

        // Knight
        add_moves(list, count, pieces[KNIGHT] & colors[stm], targets, occupied, knight);

        // Bishop
        add_moves(list, count, (pieces[BISHOP] | pieces[QUEEN]) & colors[stm], targets, occupied, bishop);

        // Rook
        add_moves(list, count, (pieces[ROOK] | pieces[QUEEN]) & colors[stm], targets, occupied, rook);

        // King
        add_moves(list, count, pieces[KING] & colors[stm], targets, occupied, king);

        // Castling
        if (is_all && !checkers) {
            int castling_rights = ~castled >> stm * 2;

            if (castling_rights & 1 && !(occupied & 0x60ull << stm * 56)) list[count++] = move_make(E1 + stm * 56, G1 + stm * 56);
            if (castling_rights & 2 && !(occupied & 0xeull << stm * 56)) list[count++] = move_make(E1 + stm * 56, C1 + stm * 56);
        }

        return count;
    }

    int eval() {
        int eval = 0;
        int phase = 0;

        for (int color = WHITE; color < 2; color++) {
            u64 pawns[] = { pieces[PAWN] & colors[WHITE], pieces[PAWN] & colors[BLACK] };
            u64 pawns_threats = se(pawns[!color]) | sw(pawns[!color]);
            u64 pawns_attacks = ne(pawns[color]) | nw(pawns[color]);
            u64 pawns_phalanx = west(pawns[color]) & pawns[color];

            // Bishop pair
            eval += (POPCNT(pieces[BISHOP] & colors[color]) > 1) * BISHOP_PAIR;

            // Pawn protected
            eval += POPCNT(pawns[color] & pawns_attacks) * PAWN_PROTECTED;

            for (int type = PAWN; type < TYPE_NONE; type++) {
                u64 mask = pieces[type] & colors[color];

                while (mask) {
                    int square = LSB(mask);
                    mask &= mask - 1;

                    // Material + PST
                    eval +=
                        MATERIAL[type] + 
                        (get_data(type * 8 + square / 8) + OFFSET_PST_RANK) * SCALE_PST +
                        (get_data(type * 8 + square % 8 + INDEX_PST_FILE) + OFFSET_PST_FILE) * SCALE_PST;

                    // Update phase
                    phase += PHASE[type];

                    // Pawn threats
                    if (1ull << square & pawns_threats)
                        eval -= get_data(type + INDEX_THREAT) + OFFSET_THREAT;

                    if (!type) {
                        // Pawn phalanx
                        if (pawns_phalanx & 1ull << square)
                            eval += (get_data(square / 8 + INDEX_PHALANX) + OFFSET_PHALANX) * SCALE_PHALANX;

                        // Passed pawns
                        if (!(0x101010101010101ull << square & (pawns[!color] | pawns_threats)))
                            eval += (get_data(square / 8 + INDEX_PASSER) + OFFSET_PASSER) * SCALE_PASSER;
                    }
                    else {
                        // Mobility
                        u64 mobility =
                            type < BISHOP ? knight(1ull << square) :
                            type > QUEEN ? king(1ull << square) :
                            (type != BISHOP) * rook(1ull << square, colors[WHITE] | colors[BLACK]) |
                            (type != ROOK) * bishop(1ull << square, colors[WHITE] | colors[BLACK]);

                        eval += (get_data(type + INDEX_MOBILITY) + OFFSET_MOBILITY) * POPCNT(mobility & ~colors[color] & ~pawns_threats);

                        // Open file
                        if (!(0x101010101010101ull << square % 8 & pieces[PAWN]))
                            eval += (type > QUEEN) * KING_OPEN + (type == ROOK) * ROOK_OPEN;

                        // Semi open file
                        if (!(0x101010101010101ull << square % 8 & pawns[color]))
                            eval += (type > QUEEN) * KING_SEMI_OPEN + (type == ROOK) * ROOK_SEMI_OPEN;

                        // Pawn shield
                        if (type > QUEEN && square < A2)
                            eval += POPCNT(pawns[color] & 0x70700ull << 5 * (square % 8 > 2)) * PAWN_SHIELD;
                    }
                }
            }

            // Flip board
            colors[WHITE] = BSWAP(colors[WHITE]);
            colors[BLACK] = BSWAP(colors[BLACK]);

            for (int type = PAWN; type < TYPE_NONE; type++)
                pieces[type] = BSWAP(pieces[type]);

            eval = -eval;
        }

        // Scaling
        int x = 8 - POPCNT(pieces[PAWN] & colors[eval < 0]);

        eval = (i16(eval) * phase + (eval + 0x8000 >> 16) * (128 - x * x) / 128 * (24 - phase)) / 24;

        return (stm ? -eval : eval) + TEMPO;
    }

#ifdef OB_MINI
    void from_fen(istream& fen) {
        memset(this, 0, sizeof(Board));
        memset(board, PIECE_NONE, 64);

        string token;

        // Set pieces
        fen >> token;

        int square = 56;

        for (char c : token) {
            if (isdigit(c)) {
                square += c - '0';
            }
            else if (c == '/') {
                square -= 16;
            }
            else {
                int piece =
                    c == 'P' ? WHITE_PAWN :
                    c == 'N' ? WHITE_KNIGHT :
                    c == 'B' ? WHITE_BISHOP :
                    c == 'R' ? WHITE_ROOK :
                    c == 'Q' ? WHITE_QUEEN :
                    c == 'K' ? WHITE_KING :
                    c == 'p' ? BLACK_PAWN :
                    c == 'n' ? BLACK_KNIGHT :
                    c == 'b' ? BLACK_BISHOP :
                    c == 'r' ? BLACK_ROOK :
                    c == 'q' ? BLACK_QUEEN :
                    c == 'k' ? BLACK_KING :
                    PIECE_NONE;

                edit(square, piece);

                square += 1;
            }
        }

        // Side to move
        fen >> token;

        if (token == "b") {
            stm = BLACK;
            hash ^= KEYS[PIECE_NONE][0];
        }

        // Castling rights
        fen >> token;

        castled = CASTLED_WK | CASTLED_WQ | CASTLED_BK | CASTLED_BQ;

        for (char c : token) {
            if (c == 'K') castled ^= CASTLED_WK;
            if (c == 'Q') castled ^= CASTLED_WQ;
            if (c == 'k') castled ^= CASTLED_BK;
            if (c == 'q') castled ^= CASTLED_BQ;
        }

        hash ^= KEYS[PIECE_NONE][castled];

        // Enpassant square
        fen >> token;

        enpassant = SQUARE_NONE;

        if (token != "-") {
            enpassant = (token[1] - '1') * 8 + token[0] - 'a' + A1;
            hash ^= KEYS[PIECE_NONE][enpassant];
        }

        // Halfmove counter
        fen >> token;

        halfmove = stoi(token);

        // Fullmove counter;
        fen >> token;

        // Check
        checkers = attackers(LSB(pieces[KING] & colors[stm])) & colors[!stm];
    }
#endif

    Board() {
        memset(this, 0, sizeof(Board));
        memset(board, PIECE_NONE, 64);

        enpassant = SQUARE_NONE;

        for (int i = 0; i < 8; i++) {
            edit(i + A1, LAYOUT[i] * 2 | WHITE);
            edit(i + A8, LAYOUT[i] * 2 | BLACK);
            edit(i + A2, WHITE_PAWN);
            edit(i + A7, BLACK_PAWN);
        }
    }
};