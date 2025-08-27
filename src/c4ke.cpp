#include <bits/stdc++.h>

using namespace std;

using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using u64 = uint64_t;

// Constants
#define TRUE 1
#define FALSE 0

#define WHITE 0
#define BLACK 1

#define PAWN 0
#define KNIGHT 1
#define BISHOP 2
#define ROOK 3
#define QUEEN 4
#define KING 5
#define TYPE_NONE 6

#define WHITE_PAWN 0
#define BLACK_PAWN 1
#define WHITE_KNIGHT 2
#define BLACK_KNIGHT 3
#define WHITE_BISHOP 4
#define BLACK_BISHOP 5
#define WHITE_ROOK 6
#define BLACK_ROOK 7
#define WHITE_QUEEN 8
#define BLACK_QUEEN 9
#define WHITE_KING 10
#define BLACK_KING 11
#define PIECE_NONE 12

#define A1 0
#define B1 1
#define C1 2
#define D1 3
#define E1 4
#define F1 5
#define G1 6
#define H1 7
#define A2 8
#define B2 9
#define C2 10
#define D2 11
#define E2 12
#define F2 13
#define G2 14
#define H2 15
#define A3 16
#define B3 17
#define C3 18
#define D3 19
#define E3 20
#define F3 21
#define G3 22
#define H3 23
#define A4 24
#define B4 25
#define C4 26
#define D4 27
#define E4 28
#define F4 29
#define G4 30
#define H4 31
#define A5 32
#define B5 33
#define C5 34
#define D5 35
#define E5 36
#define F5 37
#define G5 38
#define H5 39
#define A6 40
#define B6 41
#define C6 42
#define D6 43
#define E6 44
#define F6 45
#define G6 46
#define H6 47
#define A7 48
#define B7 49
#define C7 50
#define D7 51
#define E7 52
#define F7 53
#define G7 54
#define H7 55
#define A8 56
#define B8 57
#define C8 58
#define D8 59
#define E8 60
#define F8 61
#define G8 62
#define H8 63
#define SQUARE_NONE 64

#define MAX_PLY 256
#define MAX_MOVE 256

#define WIN 32000
#define INF 32256
#define DRAW 0

#define CASTLED_NONE 0
#define CASTLED_WK 1
#define CASTLED_WQ 2
#define CASTLED_BK 4
#define CASTLED_BQ 8

#define BOUND_UPPER 0
#define BOUND_LOWER 1
#define BOUND_EXACT 2

#define MOVE_NONE 0

#define TT_BITS 20

#define HIST_MAX 16384

#define STACK_SIZE 264

// Time
u64 now() {
    timespec t;
    clock_gettime(1, &t);
    return t.tv_sec * 1e3 + t.tv_nsec / 1e6;
}

// Zobrist
u64 KEYS[13][64];

// Bitboard
int lsb(u64 bitboard) {
    return countr_zero(bitboard);
}

int count(u64 bitboard) {
    return popcount(bitboard);
}

u64 north(u64 bitboard) {
    return bitboard << 8;
}

u64 south(u64 bitboard) {
    return bitboard >> 8;
}

u64 west(u64 bitboard) {
    return bitboard >> 1 & 0x7F7F7F7F7F7F7F7FULL;
}

u64 east(u64 bitboard) {
    return bitboard << 1 & 0xFEFEFEFEFEFEFEFEULL;
}

u64 nw(u64 bitboard) {
    return north(west(bitboard));
}

u64 ne(u64 bitboard) {
    return north(east(bitboard));
}

u64 sw(u64 bitboard) {
    return south(west(bitboard));
}

u64 se(u64 bitboard) {
    return south(east(bitboard));
}

u64 ray(u64 mask, u64 occupied, u64 (*func)(u64)) {
    mask = func(mask);

    for (int i = 0; i < 6; i++) {
        mask |= func(mask & ~occupied);
    }

    return mask;
}

u64 knight(u64 mask, u64 occupied = 0) {
    return (mask << 6 | mask >> 10) & 0x3F3F3F3F3F3F3F3FULL | (mask << 10 | mask >> 6) & 0xFCFCFCFCFCFCFCFCULL | (mask << 17 | mask >> 15) & 0xFEFEFEFEFEFEFEFEULL | (mask << 15 | mask >> 17) & 0x7F7F7F7F7F7F7F7FULL;
}

u64 bishop(u64 mask, u64 occupied) {
    return ray(mask, occupied, nw) | ray(mask, occupied, ne) | ray(mask, occupied, sw) | ray(mask, occupied, se);
}

u64 rook(u64 mask, u64 occupied) {
    return ray(mask, occupied, north) | ray(mask, occupied, south) | ray(mask, occupied, west) | ray(mask, occupied, east);
}

u64 king(u64 mask, u64 occupied = 0) {
    return mask << 8 | mask >> 8 | (mask >> 1 | mask >> 9 | mask << 7) & 0x7F7F7F7F7F7F7F7FULL | (mask << 1 | mask << 9 | mask >> 7) & 0xFEFEFEFEFEFEFEFEULL;
}

// TODO: will minify away
void bitboard_print(u64 bitboard) {
    for (int rank = 7; rank >= 0; rank--) {
        char line[] = ". . . . . . . .";

        for (int file = 0; file < 8; file++) {
            if (1ULL << (file | (rank << 3)) & bitboard) {
                line[2 * file] = 'X';
            }
        }

        printf("%s\n", line);
    }

    printf("\n");
}

// Move
u16 move_make(int from, int to, int promo = PAWN) {
    return from | to << 6 | promo << 12;
}

int move_from(u16 move) {
    return move & 63;
}

int move_to(u16 move) {
    return move >> 6 & 63;
}

int move_promo(u16 move) {
    return move >> 12;
}

void move_print(u16 move) {
    cout.put(97 + move_from(move) % 8).put(49 + move_from(move) / 8).put(97 + move_to(move) % 8).put(49 + move_to(move) / 8).put(" nbrq"[move_promo(move)]) << endl;
}

// Board
int LAYOUT[] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };
int PIECE_VALUE[] = { 100, 320, 330, 500, 900, 2000 };
int PST_RANK[] = {
    0, -3, -3, -1, 1, 5, 0, 0,
    -2, 0, 1, 3, 4, 5, 2, -15,
    0, 2, 2, 2, 2, 2, -1, -10,
    0, -1, -2, -2, 0, 2, 1, 2,
    2, 3, 2, 0, 0, -1, -4, -2,
    -1, 1, -1, -4, -1, 5, 5, 5,
};
int PST_FILE[] = {
    -1, -2, -1, 0, 1, 2, 2, -1,
    -4, -1, 0, 2, 2, 2, 1, -1,
    -2, 0, 1, 0, 1, 0, 2, -1,
    -2, -1, 0, 1, 2, 1, 1, -1,
    -2, -1, -1, 0, 0, 1, 2, 1,
    -2, 2, -1, -4, -4, -2, 2, 0,
};

void add_pawn_moves(u16 list[], int& count, u64 targets, int offset) {
    while (targets) {
        int to = lsb(targets);
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
        int from = lsb(mask);
        mask &= mask - 1;

        u64 attack = func(1ULL << from, occupied) & targets;

        while (attack) {
            int to = lsb(attack);
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

    void edit(int square, int piece) {
        u64 mask = 1ULL << square;

        if (board[square] < PIECE_NONE) {
            hash ^= KEYS[board[square]][square];

            pieces[board[square] / 2] &= ~mask;
            colors[board[square] & 1] &= ~mask;
        }

        if (piece < PIECE_NONE) {
            hash ^= KEYS[piece][square];

            pieces[piece / 2] |= mask;
            colors[piece & 1] |= mask;
        }

        board[square] = piece;
    }

    int attacked(int square, int enemy) {
        u64 mask = 1ULL << square;
        u64 pawns = pieces[PAWN] & colors[enemy];
        u64 occupied = colors[WHITE] | colors[BLACK];

        return
            (enemy ? sw(pawns) | se(pawns) : nw(pawns) | ne(pawns)) & mask ||
            knight(mask) & colors[enemy] & pieces[KNIGHT] ||
            bishop(mask, occupied) & colors[enemy] & (pieces[BISHOP] | pieces[QUEEN]) ||
            rook(mask, occupied) & colors[enemy] & (pieces[ROOK] | pieces[QUEEN]) ||
            king(mask) & colors[enemy] & pieces[KING];
    }

    int drawn(vector<u64>& visited, int ply) {
        int before_root = FALSE;

        for (int i = 4; i <= halfmove && i <= visited.size(); i += 2) {
            if (hash == visited[visited.size() - i]) {
                if (ply >= i) return TRUE;
                if (before_root) return TRUE;
                before_root = TRUE;
            }
        }

        return halfmove > 99;
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
        is_checked = attacked(lsb(pieces[KING] & colors[stm]), !stm);

        // Check if legal
        return !attacked(lsb(pieces[KING] & colors[!stm]), stm);
    }

    int movegen(u16 list[], int is_all) {
        int count = 0;

        u64 occupied = colors[WHITE] | colors[BLACK];
        u64 targets = is_all ? ~colors[stm] : colors[!stm];

        // Pawn
        u64 pawns = pieces[PAWN] & colors[stm];
        u64 pawns_targets = colors[!stm] | u64(enpassant < SQUARE_NONE) << enpassant;
        u64 push_1 = (stm ? south(pawns) : north(pawns)) & ~occupied;

        add_pawn_moves(list, count, is_all ? push_1 : push_1 & 0xFF000000000000FFULL, stm ? -8 : 8);
        if (is_all) add_pawn_moves(list, count, (stm ? south(push_1 & 0xFFULL << 40) : north(push_1 & 0xFF0000ULL)) & ~occupied, stm ? -16 : 16);
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
            u8 castling_rights = castled >> stm * 2 ^ 3;

            if (castling_rights & 1 && !(occupied & 0x60ULL << stm * 56)) list[count++] = move_make(E1 + stm * 56, G1 + stm * 56);
            if (castling_rights & 2 && !(occupied & 0xEULL << stm * 56)) list[count++] = move_make(E1 + stm * 56, C1 + stm * 56);
        }

        return count;
    }

    int eval() {
        int eval = 0;

        for (int type = PAWN; type < KING; type++) {
            eval += PIECE_VALUE[type] * count(pieces[type] & colors[WHITE]);
            eval -= PIECE_VALUE[type] * count(pieces[type] & colors[BLACK]);
        }

        for (int square = A1; square < 64; square++) {
            if (board[square] < PIECE_NONE) {
                int type = board[square] / 2;
                int flip = board[square] & 1;
                int index = square ^ (flip * 56);

                int rank = index / 8;
                int file = index % 8;

                eval += (PST_RANK[type * 8 + rank] + PST_FILE[type * 8 + file]) * (flip ? -8 : 8);
            }
        }

        return (stm ? -eval : eval) + 20;
    }

    // TODO: will minify away
    void print() {
        char piece_str[] = {
            'P', 'p',
            'N', 'n',
            'B', 'b',
            'R', 'r',
            'Q', 'q',
            'K', 'k'
        };

        for (int rank = 7; rank >= 0; rank--) {
            char line[] = ". . . . . . . .";

            for (int file = 0; file < 8; file++) {
                int square = file | (rank << 3);

                if (board[square] == PIECE_NONE) {
                    continue;
                }

                line[2 * file] = piece_str[board[square]];
            }

            printf("%s\n", line);
        }

        printf("\n");

        cout << "stm: " << int(stm) << "\n";
        cout << "ep: " << int(enpassant) << "\n";
        cout << "hm: " << halfmove << "\n";
        cout << "castled: " << bitset<4>(castled) << "\n";
    }

    // TODO: will minify away
    void from_fen(stringstream fen) {
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
    }

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

// History
void update_history(i16& entry, int bonus) {
    entry += bonus - entry * abs(bonus) / HIST_MAX;
}

// Shared state
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

// Search
struct Stack {
    int eval = INF;
    u16 move = MOVE_NONE;
    u16 killer = MOVE_NONE;
};

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
        TTEntry tt = slot;

        tt.hash ^= board.hash;

        if (!tt.hash && !is_pv && depth <= tt.depth && tt.bound != tt.score < beta) {
            return tt.score;
        }

        // Clear killer
        stack[ply + 1].killer = MOVE_NONE;

        // Best score
        int best = -INF;
        u16 best_move = MOVE_NONE;

        u8 bound = BOUND_UPPER;
        
        // Standpat
        if (is_qsearch && !board.is_checked) {
            best = board.eval();

            if (best >= beta)
                return best;

            if (alpha < best)
                alpha = best;
        }

        // Generate move
        u16 move_list[MAX_MOVE];
        int move_scores[MAX_MOVE];
        int move_count = board.movegen(move_list, !is_qsearch || board.is_checked);

        // Score move
        for (int i = 0; i < move_count; i++) {
            int victim = board.board[move_to(move_list[i])] / 2;

            // Hash move
            if (move_list[i] == tt.move)
                move_scores[i] = 1e8;
            // Noisy moves
            else if (victim < TYPE_NONE || move_promo(move_list[i]))
                move_scores[i] = PIECE_VALUE[victim] * 16 - PIECE_VALUE[board.board[move_from(move_list[i])] / 2] * 8 + 1e7;
            // Quiet moves
            else
                move_scores[i] = move_list[i] == stack[ply].killer ? 1e6 : qhist[move_list[i] & 4095];
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
            int is_quiet = board.board[move_to(move)] < PIECE_NONE || move_promo(move);

            // Make
            Board child = board;

            if (!child.make(move))
                continue;

            legals++;

            visited.push_back(child.hash);

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
        slot = TTEntry { u8(board.hash), best_move, i16(best), u8(!is_qsearch * depth), bound };

        return best;
    }

    void start(Board board, vector<u64>& pre_visited, u64 time) {
        // Set data
        nodes = 0;
        pv = MOVE_NONE;
        visited = pre_visited;
        memset(qhist, 0, sizeof(qhist));

        // Iterative deepening
        for (int depth = 1; depth < MAX_PLY; ++depth) {
            // Clear stack
            for (Stack& ss : stack) ss = Stack();

            // Search
            int score = search(board, -INF, INF, 0, depth, TRUE);

            // Print info
            cout << "info depth " << depth << " score cp " << score << " pv ";
            move_print(pv);

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

int main() {
    // Zobrist hash init
    for (int i = 0; i < 13; i++)
        for (int k = 0; k < 64; k++)
            KEYS[i][k] = rand() | rand() << 16 | (u64)rand() << 32 | (u64)rand() << 48;

    // Search data
    Board board;
    vector<u64> visited(512);
    TTABLE = (TTEntry*)calloc(1ULL << TT_BITS, 8);

    // Uci
    string token;

    cin >> token;
    cout << "uciok\n";

    while (getline(cin, token)) {
        stringstream tokens(token);
        tokens >> token;

        if (token[0] == 'i') {
            cout << "readyok\n";
        }
        else if (token[0] == 'p') {
            board = Board();
            visited.clear();

            tokens >> token >> token;

            while (tokens >> token) {
                visited.push_back(board.hash);
                board.make(move_make(token[0] + token[1] * 8 - 489, token[2] + token[3] * 8 - 489, token[4] % 35 * 5 % 6));
            }
        }
        else if (token[0] == 'g') {
            u64 time;

            tokens >> token >> time;

            if (board.stm)
                tokens >> token >> time;

            RUNNING = TRUE;
            LIMIT_SOFT = now() + time / 50;
            LIMIT_HARD = now() + time / 2;

            thread t([&] () {
                Thread().start(board, visited, time);
            });

            t.join();

            cout << "bestmove ";
            move_print(BEST_MOVE);
        }
        else if (token[0] == 'q') {
            break;
        }
    }

    free(TTABLE);

    return 0;
};