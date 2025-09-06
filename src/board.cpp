#include "eval.cpp"

int LAYOUT[] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };

void add_pawn_moves(u16 list[], int& count, u64 targets, int offset) {
    while (targets) {
        int to = __builtin_ctzll(targets);
        int from = to - offset;

        targets &= targets - 1;

        if (to < 8 || to > 55) {
            list[count++] = move_make(from, to, KNIGHT);
            list[count++] = move_make(from, to, BISHOP);
            list[count++] = move_make(from, to, ROOK);
            list[count++] = move_make(from, to, QUEEN);
        }
        else {
            list[count++] = move_make(from, to);
        }
    }
}

void add_moves(u16 list[], int& count, u64 mask, u64 targets, u64 occupied, u64 (*func)(u64, u64)) {
    while (mask) {
        int from = __builtin_ctzll(mask);
        mask &= mask - 1;

        u64 attack = func(1ull << from, occupied) & targets;

        while (attack) {
            int to = __builtin_ctzll(attack);
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
        u64 mask = 1ull << square;
        u64 occupied = colors[WHITE] | colors[BLACK];

        return
            (nw(mask) | ne(mask)) & pieces[PAWN] & colors[BLACK] |
            (sw(mask) | se(mask)) & pieces[PAWN] & colors[WHITE] |
            knight(mask) & pieces[KNIGHT] |
            bishop(mask, occupied) & (pieces[BISHOP] | pieces[QUEEN]) |
            rook(mask, occupied) & (pieces[ROOK] | pieces[QUEEN]) |
            king(mask) & pieces[KING];
    }

    int quiet(u16 move) {
        return board[move_to(move)] > BLACK_KING && !move_promo(move) && !(board[move_from(move)] < WHITE_KNIGHT && move_to(move) == enpassant);
    }

    int make(u16 move) {
        // Get move data
        int from = move_from(move);
        int to = move_to(move);
        int piece = board[from];

        // Check enpassant
        int is_enpassant = to == enpassant;

        // Update halfmove
        halfmove += board[to] > BLACK_KING;

        // Update enpassant square
        if (enpassant < SQUARE_NONE)
            hash ^= KEYS[PIECE_NONE][enpassant];
        
        enpassant = SQUARE_NONE;

        // Move piece
        edit(to, move_promo(move) ? move_promo(move) * 2 + stm : piece);
        edit(from, PIECE_NONE);

        // Pawn move
        if (piece < WHITE_KNIGHT) {
            halfmove = 0;

            // Enpassant
            if (is_enpassant) {
                edit(to ^ 8, PIECE_NONE);
                hash ^= KEYS[PIECE_NONE][to];
            }

            // Double push
            if (abs(from - to) == 16) {
                enpassant = to ^ 8;
                hash ^= KEYS[PIECE_NONE][enpassant];
            }
        }

        // Castling
        hash ^= KEYS[PIECE_NONE][castled];

        if (piece > BLACK_QUEEN) {
            if (abs(from - to) == 2) {
                int dt = to > from ? 1 : -1;

                if ((attackers(from + dt) | attackers(from + dt * 2)) & colors[!stm])
                    return FALSE;

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
        checkers = attackers(__builtin_ctzll(pieces[KING] & colors[stm])) & colors[!stm];

        // Check if legal
        return !(attackers(__builtin_ctzll(pieces[KING] & colors[!stm])) & colors[stm]);
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
        add_pawn_moves(list, count, (stm ? south(pawns_push & 0xffull << 40) : north(pawns_push & 0xff0000ull)) & ~occupied, stm ? -16 : 16);
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
            // Enemy pawn attacks
            u64 enemy_pawn_attacks = se(pieces[PAWN] & colors[!color]) | sw(pieces[PAWN] & colors[!color]);

            // Bishop pair
            eval += (__builtin_popcountll(pieces[BISHOP] & colors[color]) > 1) * BISHOP_PAIR;

            for (int type = PAWN; type < TYPE_NONE; type++) {
                u64 mask = pieces[type] & colors[color];

                while (mask) {
                    int square = __builtin_ctzll(mask);
                    mask &= mask - 1;

                    // Material + PST
                    eval += MATERIAL[type] + (PST_RANK[type * 8 + square / 8] + PST_FILE[type * 8 + square % 8]) * 8;
                    phase += PHASE[type];

                    if (!type) {
                        // Passed pawns
                        if (!(0x101010101010101ull << square & (pieces[PAWN] & colors[!color] | enemy_pawn_attacks)))
                            eval += PASSER[square / 8];
                    }
                    else {
                        // Mobility
                        u64 mobility =
                            type < BISHOP ? knight(1ULL << square) :
                            type > QUEEN ? king(1ULL << square) :
                            (type != BISHOP) * rook(1ULL << square, colors[WHITE] | colors[BLACK]) |
                            (type != ROOK) * bishop(1ULL << square, colors[WHITE] | colors[BLACK]);

                        eval += MOBILITY[type] * __builtin_popcountll(mobility & ~colors[color] & ~enemy_pawn_attacks);
                    }
                }
            }

            // Flip board
            colors[WHITE] = __builtin_bswap64(colors[WHITE]);
            colors[BLACK] = __builtin_bswap64(colors[BLACK]);

            for (u64& type : pieces)
                type = __builtin_bswap64(type);

            eval = -eval;
        }

        eval = (i16(eval) * phase + (eval + 0x8000 >> 16) * (24 - phase)) / 24;

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
        checkers = attackers(__builtin_ctzll(pieces[KING] & colors[stm])) & colors[!stm];
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