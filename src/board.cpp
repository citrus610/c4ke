#include "chess.cpp"

int PIECE_VALUE[] = { 100, 320, 330, 500, 900, 2000, 0 };

// PST copied from 4ku, will replace later
int PST_RANK[] = {
    0, -3, -3, -1, 1, 5, 0, 0,
    -2, 0, 1, 3, 4, 5, 2, -15,
    0, 2, 2, 2, 2, 2, -1, -10,
    0, -1, -2, -2, 0, 2, 1, 2,
    2, 3, 2, 0, 0, -1, -4, -2,
    -1, 1, -1, -4, -1, 5, 5, 5
};

int PST_FILE[] = {
    -1, -2, -1, 0, 1, 2, 2, -1,
    -4, -1, 0, 2, 2, 2, 1, -1,
    -2, 0, 1, 0, 1, 0, 2, -1,
    -2, -1, 0, 1, 2, 1, 1, -1,
    -2, -1, -1, 0, 0, 1, 2, 1,
    -2, 2, -1, -4, -4, -2, 2, 0
};

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
    u8 stm;
    u8 castled;
    u8 enpassant;
    int is_checked;
    int halfmove;
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

    int attacked(int square, int enemy) {
        u64 mask = 1ull << square;
        u64 pawns = pieces[PAWN] & colors[enemy];
        u64 occupied = colors[WHITE] | colors[BLACK];

        return
            (enemy ? sw(pawns) | se(pawns) : nw(pawns) | ne(pawns)) & mask ||
            knight(mask) & colors[enemy] & pieces[KNIGHT] ||
            bishop(mask, occupied) & colors[enemy] & (pieces[BISHOP] | pieces[QUEEN]) ||
            rook(mask, occupied) & colors[enemy] & (pieces[ROOK] | pieces[QUEEN]) ||
            king(mask) & colors[enemy] & pieces[KING];
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
        halfmove += board[to] == PIECE_NONE;

        // Update enpassant square
        if (enpassant < SQUARE_NONE)
            hash ^= KEYS[PIECE_NONE][enpassant];
        
        enpassant = SQUARE_NONE;

        // Move piece
        edit(to, move_promo(move) ? move_promo(move) * 2 + stm : piece);
        edit(from, PIECE_NONE);

        // Pawn move
        if (piece / 2 == PAWN) {
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

        if (piece / 2 == KING) {
            if (abs(from - to) == 2) {
                int dt = to > from ? 1 : -1;

                if (attacked(from + dt, !stm) || attacked(from + dt * 2, !stm))
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
        is_checked = attacked(__builtin_ctzll(pieces[KING] & colors[stm]), !stm);

        // Check if legal
        return !attacked(__builtin_ctzll(pieces[KING] & colors[!stm]), stm);
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
        if (is_all && !is_checked) {
            u8 castling_rights = ~castled >> stm * 2;

            if (castling_rights & 1 && !(occupied & 0x60ull << stm * 56)) list[count++] = move_make(E1 + stm * 56, G1 + stm * 56);
            if (castling_rights & 2 && !(occupied & 0xeull << stm * 56)) list[count++] = move_make(E1 + stm * 56, C1 + stm * 56);
        }

        return count;
    }

    int eval() {
        int eval = 0;

        for (int type = PAWN; type < KING; type++) {
            eval += PIECE_VALUE[type] * __builtin_popcountll(pieces[type] & colors[WHITE]);
            eval -= PIECE_VALUE[type] * __builtin_popcountll(pieces[type] & colors[BLACK]);
        }

        for (int square = A1; square < 64; square++) {
            if (board[square] < PIECE_NONE) {
                int type = board[square] / 2;
                int flip = board[square] & 1;
                int index = square ^ flip * 56;

                int rank = index / 8;
                int file = index % 8;

                eval += (PST_RANK[type * 8 + rank] + PST_FILE[type * 8 + file]) * (flip ? -8 : 8);
            }
        }

        return (stm ? -eval : eval) + 20;
    }

#ifdef OB
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
        is_checked = attacked(__builtin_ctzll(pieces[KING] & colors[stm]), !stm);
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