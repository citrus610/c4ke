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

#define CASTLED_NONE 0
#define CASTLED_WK 1
#define CASTLED_WQ 2
#define CASTLED_BK 4
#define CASTLED_BQ 8

#define BOUND_UPPER 0
#define BOUND_LOWER 1
#define BOUND_EXACT 2

#define MOVE_NORMAL 0
#define MOVE_PROMOTION 1
#define MOVE_ENPASSANT 2
#define MOVE_CASTLING 3

#define TT_BIT 20

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
    return (ray(mask, occupied, nw) | ray(mask, occupied, ne) | ray(mask, occupied, sw) | ray(mask, occupied, se)) ^ mask;
}

u64 rook(u64 mask, u64 occupied) {
    return (ray(mask, occupied, north) | ray(mask, occupied, south) | ray(mask, occupied, west) | ray(mask, occupied, east)) ^ mask;
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

string move_str(u16 move) {
    string str;

    str += 97 + move_from(move) % 8;
    str += 49 + (move_from(move) / 8);
    str += 97 + move_to(move) % 8;
    str += 49 + (move_to(move) / 8);

    if (move_promo(move)) str += " nbrq"[move_promo(move)];

    return str;
}

// Transposition
struct TTEntry {
    u16 hash;
    u16 move;
    i16 score;
    u8 depth;
    u8 bound;
};

vector<TTEntry> ttable;

// Board
int LAYOUT[] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };

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

    int is_attacked(int square, int enemy) {
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

    int make(u16 move) {
        // Get move data
        int from = move_from(move);
        int to = move_to(move);
        int piece = board[from];

        // Check move type
        int is_castling = board[to] == 6 + stm; // ROOK * 2 = 6
        int is_enpassant = to == enpassant;

        // Update halfmove
        halfmove += board[to] == PIECE_NONE || !is_castling;

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
            if (is_castling) {
                int dt = to > from ? 1 : -1;

                if (is_attacked(from + dt, !stm) || is_attacked(from + dt * 2, !stm))
                    return FALSE;

                edit(to, PIECE_NONE);
                edit(from + dt, ROOK * 2 + stm);
                edit(from + dt * 2, piece);
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
        is_checked = is_attacked(lsb(pieces[KING] & colors[stm]), !stm);

        // Check if legal
        return !is_attacked(lsb(pieces[KING] & colors[!stm]), stm);
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

            if (castling_rights & 1 && !(occupied & 0x60ULL << stm * 56)) list[count++] = move_make(E1 + stm * 56, H1 + stm * 56);
            if (castling_rights & 2 && !(occupied & 0xEULL << stm * 56)) list[count++] = move_make(E1 + stm * 56, A1 + stm * 56);
        }

        return count;
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
            enpassant = (token[1] - '1') * 10 + token[0] - 'a' + A1;
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

// Time
u64 now() {
    timespec t;
    clock_gettime(1, &t);
    return t.tv_sec * 1e4 + t.tv_nsec / 1e6;
}

// TODO: will minify away
u64 perft(Board& board, int depth, bool is_root = false) {
    if (depth <= 0) {
        return 1;
    }

    u64 nodes = 0;

    u16 moves[MAX_MOVE];
    int count = board.movegen(moves, TRUE);

    for (int i = 0; i < count; i++) {
        Board child = board;

        if (!child.make(moves[i])) {
            continue;
        }

        u64 children = perft(child, depth - 1);

        nodes += children;

        if (is_root) {
            cout << move_str(moves[i]) << " - " << children << std::endl;
        }
    }

    return nodes;
}

int main() {
    // Zobrist hash init
    for (int i = 0; i < 13; i++)
        for (int k = 0; k < 64; k++)
            KEYS[i][k] = rand() | rand() << 16 | (u64)rand() << 32 | (u64)rand() << 48;

    auto b = Board();

    for (int depth = 1; depth < 8; depth++)
        cout << perft(b, depth, true) << "\n";

    return 0;
};