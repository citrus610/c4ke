#include "eval.cpp"

struct Board {
    u64 pieces[6],
        colors[2];
    u8 board[64];
    i32 stm,
        castled,
        enpassant = SQUARE_NONE,
        halfmove;
    u64 checkers,
        hash,
        hash_pawn,
        hash_non_pawn[2];

    void edit(i32 square, i32 piece) {
        // Remove any pieces that exist in this square
        if (board[square] < PIECE_NONE)
            hash ^= KEYS[board[square]][square],

            pieces[board[square] / 2] ^= 1ull << square,
            colors[board[square] & 1] ^= 1ull << square,

            (board[square] / 2 < KNIGHT ? hash_pawn : hash_non_pawn[board[square] & 1]) ^= KEYS[board[square]][square];

        // Place new piece
        if (piece < PIECE_NONE)
            hash ^= KEYS[piece][square],

            pieces[piece / 2] ^= 1ull << square,
            colors[piece & 1] ^= 1ull << square,

            (piece / 2 < KNIGHT ? hash_pawn : hash_non_pawn[piece & 1]) ^= KEYS[piece][square];

        board[square] = piece;
    }

    u64 attackers(i32 square) {
        return
            (nw(1ull << square) | ne(1ull << square)) & pieces[PAWN] & colors[BLACK] |
            (sw(1ull << square) | se(1ull << square)) & pieces[PAWN] & colors[WHITE] |
            knight(1ull << square) & pieces[KNIGHT] |
            bishop(1ull << square, colors[WHITE] | colors[BLACK]) & (pieces[BISHOP] | pieces[QUEEN]) |
            rook(1ull << square, colors[WHITE] | colors[BLACK]) & (pieces[ROOK] | pieces[QUEEN]) |
            king(1ull << square) & pieces[KING];
    }

    i32 quiet(i16 move) {
        return board[move_to(move)] > BLACK_KING && !move_promo(move) && !(board[move_from(move)] < WHITE_KNIGHT && move_to(move) == enpassant);
    }

    i32 see(i16 move, i32 threshold) {
        // Move data
        i32 from = move_from(move),
            to = move_to(move),
            side = !stm;

        // Skip special moves such as promo and enpassant
        // We don't have to handle castling here since king moves are always safe
        if (move_promo(move) || board[from] < WHITE_KNIGHT && to == enpassant)
            return TRUE;

        // Return early if capturing this piece can't beat the threshold
        if ((threshold -= VALUE[board[to] / 2]) > 0)
            return FALSE;

        // Return early if we still beat the threshold after losing the piece
        if ((threshold += VALUE[board[from] / 2]) <= 0)
            return TRUE;

        // Record the original colors masks
        u64 whites = colors[WHITE],
            blacks = colors[BLACK];

        // Remove the moving piece
        colors[stm] ^= 1ull << from;

        // Loop until one side runs out of attackers, or fail to beat the threshold
        for (; u64 threats = attackers(to) & colors[side];) {
            // Get the least valuable attacker
            i32 type = PAWN;

            for (; type < KING && !(pieces[type] & threats); type++);

            // Flip side to move
            side ^= 1;

            // Negamax
            // Check if we beat the threshold
            if ((threshold = VALUE[type] - threshold) < 0) {
                side ^= type == KING && attackers(to) & colors[side];
                break;
            }

            // Remove this attacker
            colors[!side] ^= 1ull << LSB(pieces[type] & threats);
        }
        
        // Restore the original colors masks
        colors[WHITE] = whites;
        colors[BLACK] = blacks;

        return side != stm;
    }

    u64 make(i16 move) {
        // Get move data
        i32 from = move_from(move),
            to = move_to(move),
            piece = board[from];

        // Update halfmove
        halfmove += board[to] > BLACK_KING && piece > BLACK_PAWN ? 1 : -halfmove;

        // Move piece
        edit(to, move_promo(move) ? move_promo(move) * 2 + stm : piece);
        edit(from, PIECE_NONE);

        // Enpassant
        hash ^= KEYS[PIECE_NONE][enpassant];

        if (piece < WHITE_KNIGHT && to == enpassant)
            edit(to ^ 8, PIECE_NONE), hash ^= KEYS[PIECE_NONE][enpassant];

        enpassant = piece < WHITE_KNIGHT && abs(from - to) == 16 ? to ^ 8 : SQUARE_NONE;

        hash ^= KEYS[PIECE_NONE][enpassant];

        // Castling
        hash ^= KEYS[PIECE_NONE][castled];

        if (piece > BLACK_QUEEN && (castled |= 3 << stm * 2) && abs(from - to) == 2) {
            i32 dt = (from + to) / 2;

            if ((attackers(dt) | attackers(to)) & colors[!stm])
                return TRUE;

            edit(to + (to > from ? 1 : -2), PIECE_NONE);
            edit(dt, ROOK * 2 + stm);
        }

        if (from == H1 || to == H1) castled |= CASTLED_WK;
        if (from == A1 || to == A1) castled |= CASTLED_WQ;
        if (from == H8 || to == H8) castled |= CASTLED_BK;
        if (from == A8 || to == A8) castled |= CASTLED_BQ;

        hash ^= KEYS[PIECE_NONE][castled];
        hash ^= KEYS[PIECE_NONE][0];

        __builtin_prefetch(&TTABLE[hash >> TT_SHIFT]);

        // Update side to move
        stm ^= 1;

        // In check
        checkers = attackers(LSB(pieces[KING] & colors[stm])) & colors[!stm];

        // Check if not legal
        return attackers(LSB(pieces[KING] & colors[!stm])) & colors[stm];
    }

    void add_pawn_moves(i16*& list_end, u64 targets, i32 offset) {
        for (; targets;) {
            i32 to = LSB(targets);
            targets &= targets - 1;

            if (to < 8 || to > 55)
                // Prmotion
                *list_end++ = move_make(to - offset, to, KNIGHT),
                *list_end++ = move_make(to - offset, to, BISHOP),
                *list_end++ = move_make(to - offset, to, ROOK),
                *list_end++ = move_make(to - offset, to, QUEEN);
            else
                *list_end++ = move_make(to - offset, to);
        }
    }

    void add_moves(i16*& list_end, u64 mask, u64 targets, u64 occupied, auto func) {
        for (; mask;) {
            i32 from = LSB(mask);
            mask &= mask - 1;

            u64 attack = func(1ull << from, occupied) & targets;

            for (; attack;) {
                i32 to = LSB(attack);
                attack &= attack - 1;

                *list_end++ = move_make(from, to);
            }
        }
    }

    i32 movegen(i16* list, i32 is_all) {
        i16* list_end = list;

        u64 occupied = colors[WHITE] | colors[BLACK],
            targets = is_all ? ~colors[stm] : colors[!stm],
            pawns = pieces[PAWN] & colors[stm],
            pawns_push = (stm ? south(pawns) : north(pawns)) & ~occupied & (is_all ? ~0ull : 0xff000000000000ff),
            pawns_targets = colors[!stm] | u64(enpassant < SQUARE_NONE) << enpassant;

        // Pawn
        add_pawn_moves(list_end, pawns_push, stm ? -8 : 8);
        add_pawn_moves(list_end, (stm ? south(pawns_push & 0xff0000000000) : north(pawns_push & 0xff0000ull)) & ~occupied, stm ? -16 : 16);
        add_pawn_moves(list_end, (stm ? se(pawns) : nw(pawns)) & pawns_targets, stm ? -7 : 7);
        add_pawn_moves(list_end, (stm ? sw(pawns) : ne(pawns)) & pawns_targets, stm ? -9 : 9);

        // Knight
        add_moves(list_end, pieces[KNIGHT] & colors[stm], targets, occupied, knight);

        // Bishop
        add_moves(list_end, (pieces[BISHOP] | pieces[QUEEN]) & colors[stm], targets, occupied, bishop);

        // Rook
        add_moves(list_end, (pieces[ROOK] | pieces[QUEEN]) & colors[stm], targets, occupied, rook);

        // King
        add_moves(list_end, pieces[KING] & colors[stm], targets, occupied, king);

        // Castling
        if (is_all && !checkers) {
            if (~castled >> stm * 2 & 1 && !(occupied & 0x60ull << stm * 56)) *list_end++ = move_make(E1 + stm * 56, G1 + stm * 56);
            if (~castled >> stm * 2 & 2 && !(occupied & 0xeull << stm * 56)) *list_end++ = move_make(E1 + stm * 56, C1 + stm * 56);
        }

        return list_end - list;
    }

    i32 eval() {
        i32 eval = 0,
            phase = 0;

        for (i32 color = WHITE; color < 2; color++) {
            u64 pawns_us = pieces[PAWN] & colors[color],
                pawns_them = pieces[PAWN] & colors[!color],
                pawns_threats = se(pawns_them) | sw(pawns_them),
                pawns_attacks = ne(pawns_us) | nw(pawns_us),
                pawns_phalanx = west(pawns_us) & pawns_us;

            eval +=
                // Bishop pair
                (POPCNT(pieces[BISHOP] & colors[color]) > 1) * BISHOP_PAIR +
                // Pawn protected
                POPCNT(pawns_us & pawns_attacks) * PAWN_PROTECTED -
                // Pawn doubled
                POPCNT(pawns_us & (north(pawns_us) | north(north(pawns_us)))) * PAWN_DOUBLED;

            for (i32 type = PAWN; type < TYPE_NONE; type++) {
                u64 mask = pieces[type] & colors[color];

                for (; mask;) {
                    i32 square = LSB(mask);
                    mask &= mask - 1;

                    // PST
                    eval += MATERIAL[type] + (get_data(type * 8 + square / 8) + get_data(type * 8 + square % 8 + INDEX_PST_FILE) + OFFSET_PST) * SCALE;

                    // Update phase
                    phase += PHASE[type];

                    if (!type) {
                        // Pawn phalanx
                        if (pawns_phalanx & 1ull << square)
                            eval += (get_data(square / 8 + INDEX_PHALANX) + OFFSET_PHALANX) * SCALE;

                        // Passed pawns
                        if (!(0x101010101010101 << square & (pawns_them | pawns_threats)))
                            eval += (get_data(square / 8 + INDEX_PASSER) + OFFSET_PASSER) * SCALE;
                    }
                    else {
                        // Mobility
                        u64 mobility =
                            type < BISHOP ? knight(1ull << square) :
                            type > QUEEN ? king(1ull << square) :
                            (type > BISHOP) * rook(1ull << square, colors[WHITE] | colors[BLACK]) |
                            (type != ROOK) * bishop(1ull << square, colors[WHITE] | colors[BLACK]);

                        eval += (get_data(type + INDEX_MOBILITY) + OFFSET_MOBILITY) * POPCNT(mobility & ~colors[color] & ~pawns_threats);

                        // Open file
                        if (!(0x101010101010101 << square % 8 & pieces[PAWN]))
                            eval += (type > QUEEN) * KING_OPEN + (type == ROOK) * ROOK_OPEN;

                        // Semi open file
                        if (!(0x101010101010101 << square % 8 & pawns_us))
                            eval += (type > QUEEN) * KING_SEMIOPEN + (type == ROOK) * ROOK_SEMIOPEN;

                        // Pawn shield
                        if (type > QUEEN && square < A2)
                            eval += POPCNT(pawns_us & 0x70700ull << 5 * (square % 8 > 2)) * PAWN_SHIELD;

                        // Pawn threats
                        if (1ull << square & pawns_threats)
                            eval -= get_data(type + INDEX_THREAT) + OFFSET_THREAT;

                        // King attacker
                        eval += POPCNT(mobility & king(pieces[KING] & colors[!color])) * (get_data(type + INDEX_KING_ATTACK) + OFFSET_KING_ATTACK) * (type < KING);
                    }
                }
            }

            // Flip board
            colors[WHITE] = BSWAP(colors[WHITE]);
            colors[BLACK] = BSWAP(colors[BLACK]);

            for (i32 type = PAWN; type < TYPE_NONE; type++)
                pieces[type] = BSWAP(pieces[type]);

            eval = -eval;
        }

        // Scaling
        i32 x = 8 - POPCNT(pieces[PAWN] & colors[eval < 0]);
        return (i16(eval = stm ? -eval : eval) * phase + (eval + 0x8000 >> 16) * (128 - x * x) / 128 * (24 - phase)) / 24 + TEMPO;
    }

#ifdef OB_MINI
    void from_fen(istream& fen) {
        memset(this, 0, sizeof(Board));
        memset(board, PIECE_NONE, 64);

        string token;

        // Set pieces
        fen >> token;

        i32 square = 56;

        for (char c : token) {
            if (isdigit(c)) {
                square += c - '0';
            }
            else if (c == '/') {
                square -= 16;
            }
            else {
                i32 piece =
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

    void startpos() {
        *this = Board{};

        for (i32 i = 0; i < 64; i++) board[i] = PIECE_NONE;

        for (i32 i = 0; i < 8; i++)
            edit(i + A1, LAYOUT[i] * 2 + WHITE),
            edit(i + A8, LAYOUT[i] * 2 + BLACK),
            edit(i + A2, WHITE_PAWN),
            edit(i + A7, BLACK_PAWN);
    }
};