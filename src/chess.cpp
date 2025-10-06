#include <bits/stdc++.h>

using namespace std;

using u8 = uint8_t;
using i16 = int16_t;
using i32 = int;
using u64 = uint64_t;

#define OB
#define OB_MINI

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

#define HIST_MAX 16384

#define CORRHIST_SIZE 16384
#define CORRHIST_BONUS_MAX 512
#define CORRHIST_BONUS_SCALE 8

#define STACK_SIZE 264
#define VISIT_SIZE 2048
#define VISIT_BYTES 16384

#ifdef OB
    i32 TT_BITS = 20;
    i32 TT_SHIFT = 64 - TT_BITS;
#else
    #define TT_BITS 20
    #define TT_SHIFT 44
#endif

#ifdef OB
    i32 THREADS = 1;
#else
    #define THREADS 1
#endif

// Time
u64 now() {
    timespec t;
    clock_gettime(1, &t);
    return t.tv_sec * 1e3 + t.tv_nsec / 1e6;
}

// Zobrist
u64 KEYS[13][65];

// Move
i16 move_make(i32 from, i32 to, i32 promo = PAWN) {
    return from | to << 6 | promo << 12;
}

i32 move_from(i16 move) {
    return move & 63;
}

i32 move_to(i16 move) {
    return move >> 6 & 63;
}

i32 move_promo(i16 move) {
    return move >> 12;
}

void move_print(i16 move) {
    cout.put(97 + move_from(move) % 8).put(49 + move_from(move) / 8).put(97 + move_to(move) % 8).put(49 + move_to(move) / 8).put(" nbrq"[move_promo(move)]) << endl;
}

// Bitboard
#define LSB __builtin_ctzll
#define POPCNT __builtin_popcountll
#define BSWAP __builtin_bswap64

u64 north(u64 bitboard) {
    return bitboard << 8;
}

u64 south(u64 bitboard) {
    return bitboard >> 8;
}

u64 west(u64 bitboard) {
    return bitboard >> 1 & ~0x8080808080808080;
}

u64 east(u64 bitboard) {
    return bitboard << 1 & ~0x101010101010101;
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

u64 ray(u64 mask, u64 occupied, auto func) {
    mask = func(mask);

    mask |= func(mask & ~occupied);
    mask |= func(mask & ~occupied);
    mask |= func(mask & ~occupied);
    mask |= func(mask & ~occupied);
    mask |= func(mask & ~occupied);
    mask |= func(mask & ~occupied);

    return mask;
}

// Get non-pawn attack mask
u64 attack(u64 mask, u64 occupied, int type) {
    // Knight
    if (type < BISHOP)
        return (mask << 6 | mask >> 10) & 0x3f3f3f3f3f3f3f3f | (mask << 10 | mask >> 6) & 0xfcfcfcfcfcfcfcfc | (mask << 17 | mask >> 15) & ~0x101010101010101 | (mask << 15 | mask >> 17) & ~0x8080808080808080;

    // King
    if (type > QUEEN)
        return mask << 8 | mask >> 8 | (mask >> 1 | mask >> 9 | mask << 7) & ~0x8080808080808080 | (mask << 1 | mask << 9 | mask >> 7) & ~0x101010101010101;
    
    // Slider
    return
        (type > BISHOP) * (ray(mask, occupied, north) | ray(mask, occupied, south) | ray(mask, occupied, west) | ray(mask, occupied, east)) |
        (type != ROOK) * (ray(mask, occupied, nw) | ray(mask, occupied, ne) | ray(mask, occupied, sw) | ray(mask, occupied, se));
}

// Shared states
struct TTEntry {
    i16 key,
        move,
        score;
    u8 depth,
        bound;
};

TTEntry* TTABLE;
i16 BEST_MOVE;
u64 LIMIT_SOFT,
    LIMIT_HARD,
    VISITED[STACK_SIZE];
i32 VISITED_COUNT;
atomic_int STOP;

#ifdef OB
void print_bitboard(u64 bitboard) {

    for (i32 rank = 7; rank >= 0; rank--) {
        char line[] = ". . . . . . . .";

        for (i32 file = 0; file < 8; file++) {
            if (bitboard & 1ull << (file | (rank << 3))) {
                line[2 * file] = 'X';
            }
        }

        printf("%s\n", line);
    }

    printf("\n");
}
#endif

// Params
int CORRHIST_PAWN_DIV = 137;
int CORRHIST_NONPAWN_DIV = 208;
int CORRHIST_CONT_1PLY_DIV = 140;
int CORRHIST_CONT_2PLY_DIV = 205;
int RFP_COEF = 66;
int NMP_EVAL_BASE = 25;
int FP_COEF = 96;
int FP_BASE = 100;
int FP_HIST_DIV = 32;
int SEEP_COEF = 81;
int SE_DEXT_MARGIN = 13;
double LMR_COEF = 0.35;
double LMR_BASE = 1.0;
int LMR_HIST_DIV = 7792;
int HIST_BONUS_COEF = 157;
int HIST_BONUS_BASE = -54;
int HIST_BONUS_MAX = 1485;
int AW_DELTA = 10;
double AW_GROWTH = 1.5;
int VALUE_PAWN = 100;
int VALUE_KNIGHT = 325;
int VALUE_BISHOP = 350;
int VALUE_ROOK = 500;
int VALUE_QUEEN = 900;

// i32 VALUE[] = { VALUE_PAWN, VALUE_KNIGHT, VALUE_BISHOP, VALUE_ROOK, VALUE_QUEEN, 2000, 0 };

// CORRHIST_PAWN_DIV, int, 137.0, 50.0, 500.0, 20.0, 0.002
// CORRHIST_NONPAWN_DIV, int, 208.0, 50.0, 500.0, 20.0, 0.002
// CORRHIST_CONT_1PLY_DIV, int, 140.0, 50.0, 500.0, 20.0, 0.002
// CORRHIST_CONT_2PLY_DIV, int, 205.0, 50.0, 500.0, 20.0, 0.002
// RFP_COEF, int, 66.0, 20.0, 200.0, 5.0, 0.002
// NMP_EVAL_BASE, int, 25.0, 10.0, 200.0, 2.5, 0.002
// FP_COEF, int, 96.0, 20.0, 200.0, 5.0, 0.002
// FP_BASE, int, 100.0,, 0.002
// FP_HIST_DIV, int, 32.0, 16.0, 128.0, 1.0, 0.002
// SEEP_COEF, int, 81.0, 20.0, 200.0, 5.0, 0.002
// SE_DEXT_MARGIN, int, 13.0, 8.0, 32.0, 2.0, 0.002
// LMR_COEF, float, 0.35, 0.1, 1.0, 0.05, 0.002
// LMR_BASE, float, 1.0, 0.5, 1.0, 0.05, 0.002
// LMR_HIST_DIV, int, 7792.0, 2000.0, 10000.0, 400.0, 0.002
// HIST_BONUS_COEF, int, 157.0, 50.0, 500.0, 8.0, 0.002
// HIST_BONUS_BASE, int, -54.0, -200.0, 200.0, 8.0, 0.002
// HIST_BONUS_MAX, int, 1485.0, 1000.0, 2000.0, 50.0, 0.002
// AW_DELTA, int, 10.0, 4.0, 20.0, 1.0, 0.002
// AW_GROWTH, float, 1.5, 1.0, 2.0, 0.05, 0.002
// VALUE_PAWN, int, 100.0, 50.0, 150.0, 5.0, 0.002
// VALUE_KNIGHT, int, 325.0, 200.0, 400.0, 10.0, 0.002
// VALUE_BISHOP, int, 350.0, 200.0, 400.0, 10.0, 0.002
// VALUE_ROOK, int, 500.0, 400.0, 600.0, 10.0, 0.002
// VALUE_QUEEN, int, 900.0, 700.0, 1500.0, 20.0, 0.002