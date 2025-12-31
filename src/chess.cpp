#include <bits/stdc++.h>

using namespace std;

using u8 = uint8_t;
using i16 = int16_t;
using i32 = int;
using u64 = uint64_t;
using null = void;

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

#define WIN 3e4
#define INF 32000
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

#define STACK_SIZE 264

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

// Move
i16 move_make(i32 from, i32 to, i32 promo = PAWN) {
    return from | to << 6 | promo << 12;
}

i32 move_from(i32 move) {
    return move & 63;
}

i32 move_to(i32 move) {
    return move >> 6 & 63;
}

i32 move_promo(i16 move) {
    return move >> 12;
}

null move_print(i32 move) {
    cout.put(97 + move_from(move) % 8).put(49 + move_from(move) / 8).put(97 + move_to(move) % 8).put(49 + move_to(move) / 8).put(move_promo(move) ? 'q' : 32) << endl;
}

// Bitboard
#define LSB countr_zero
#define POPCNT popcount
#define BSWAP byteswap

u64 BEST_MOVE,
    VISITED_COUNT,
    TIME_START,
    TIME_SOFT,
    TIME_LIMIT,
    DIAG[2][64],
    KEYS[13][65],
    VISITED[STACK_SIZE];

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

u64 hyperbola(u64 mask, u64 occupied, u64 line) {
    return line & ((occupied & line) - mask ^ BSWAP(BSWAP(occupied & line) - BSWAP(mask)));
}

// Get non-pawn attack mask
u64 attack(u64 mask, u64 occupied, i32 type) {
    // Knight
    if (type < BISHOP)
        return (mask << 6 | mask >> 10) & 0x3f3f3f3f3f3f3f3f | (mask << 10 | mask >> 6) & 0xfcfcfcfcfcfcfcfc | (mask << 17 | mask >> 15) & ~0x101010101010101 | (mask << 15 | mask >> 17) & ~0x8080808080808080;

    // King
    if (type > QUEEN)
        return mask << 8 | mask >> 8 | (mask >> 1 | mask >> 9 | mask << 7) & ~0x8080808080808080 | (mask << 1 | mask << 9 | mask >> 7) & ~0x101010101010101;
    
    // Slider
    return
        (type != ROOK) * (hyperbola(mask, occupied, DIAG[0][LSB(mask)]) | hyperbola(mask, occupied, DIAG[1][LSB(mask)])) |
        (type > BISHOP) * (hyperbola(mask, occupied, mask ^ 0x101010101010101u << LSB(mask) % 8) | ray(mask, occupied, east) | ray(mask, occupied, west));
}

// Shared states
struct TTEntry {
    i16 key,
        move,
        score;
    u8 depth,
        bound;
};

TTEntry* TTABLE = (TTEntry*)calloc(1ull << TT_BITS, 8);
atomic<i32> STOP;

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

i32 CORRHIST_PAWN_DIV = 129;
i32 CORRHIST_NONPAWN_DIV = 199;
i32 CORRHIST_CONT_1PLY = 140;
i32 CORRHIST_CONT_2PLY = 200;
i32 RAZOR_DEPTH = 26;
i32 RAZOR_COEF = 200;
i32 RFP_DEPTH = 38;
i32 RFP_COEF = 69;
i32 NMP_EVAL_MARGIN = 26;
i32 NMP_RED_BIAS = 18;
i32 NMP_RED_COEF = 14;
double CONTHIST_1PLY = 2.1;
double CONTHIST_2PLY = 2.1;
i32 FP_DEPTH = 42;
i32 FP_COEF = 86;
i32 FP_BIAS = 94;
i32 FP_HIST_DIV = 31;
i32 SEEP_COEF = 78;
i32 SE_DEPTH = 18;
i32 SE_DOUBLE_EXT_MARGIN = 10;
i32 SE_TRIPLE_EXT_MARGIN = 39;
i32 LMR_DEPTH = 10;
i32 LMR_LEGAL = 10;
double LMR_COEF = 0.35;
double LMR_BIAS = 1.0;
i32 LMR_HIST_DIV = 7561;
i32 LMR_NOISY = 6;
i32 HIST_BONUS_COEF = 168;
i32 HIST_BONUS_BIAS = -59;
i32 HIST_BONUS_MAX = 1530;
i32 HIST_EVAL_BONUS = 145;
i32 CORRHIST_BONUS_MAX = 512;
double CORRHIST_BONUS_SCALE = 8.0;
i32 ASP_DELTA = 38;
double ASP_GROWTH = 1.3;
i32 OPTIMISM_MAX = 64;
i32 VALUE_P = 111;
i32 VALUE_N = 312;
i32 VALUE_B = 290;
i32 VALUE_R = 513;
i32 VALUE_Q = 934;

// i32 VALUE[] { VALUE_P, VALUE_N, VALUE_B, VALUE_R, VALUE_Q, 5000, 0 };

// CORRHIST_PAWN_DIV, int, 129.0, 50.0, 500.0, 16.0, 0.002
// CORRHIST_NONPAWN_DIV, int, 199.0, 50.0, 500.0, 16.0, 0.002
// CORRHIST_CONT_1PLY, int, 140.0, 50.0, 500.0, 16.0, 0.002
// CORRHIST_CONT_2PLY, int, 200.0, 50.0, 500.0, 16.0, 0.002
// RAZOR_DEPTH, int, 26.0, 12.0, 64.0, 2.0, 0.002
// RAZOR_COEF, int, 200.0, 50.0, 500.0, 20.0, 0.002
// RFP_DEPTH, int, 38.0, 24.0, 64.0, 2.0, 0.002
// RFP_COEF, int, 69.0, 20.0, 200.0, 8.0, 0.002
// NMP_EVAL_MARGIN, int, 26.0, 8.0, 200.0, 3.0, 0.002
// NMP_RED_BIAS, int, 18.0, 12.0, 32.0, 2.0, 0.002
// NMP_RED_COEF, int, 14.0, 8.0, 20.0, 1.5, 0.002
// CONTHIST_1PLY, float, 2.1, 1.0, 3.0, 0.2, 0.002
// CONTHIST_2PLY, float, 2.1, 1.0, 3.0, 0.2, 0.002
// FP_DEPTH, int, 42.0, 28.0, 64.0, 3.0, 0.002
// FP_COEF, int, 86.0, 20.0, 200.0, 8.0, 0.002
// FP_BIAS, int, 94.0, 20.0, 200.0, 8.0, 0.002
// FP_HIST_DIV, int, 31.0, 16.0, 128.0, 3.0, 0.002
// SEEP_COEF, int, 78.0, 20.0, 200.0, 8.0, 0.002
// SE_DEPTH, int, 18.0, 12.0, 48.0, 2.0, 0.002
// SE_DOUBLE_EXT_MARGIN, int, 10.0, 0.0, 32.0, 1.5, 0.002
// SE_TRIPLE_EXT_MARGIN, int, 39.0, 8.0, 128.0, 4.0, 0.002
// LMR_DEPTH, int, 10.0, 4.0, 16.0, 1.0, 0.002
// LMR_LEGAL, int, 10.0, 0.0, 16.0, 1.0, 0.002
// LMR_COEF, float, 0.35, 0.25, 1.0, 0.04, 0.002
// LMR_BIAS, float, 1.0, 0.5, 2.0, 0.1, 0.002
// LMR_HIST_DIV, int, 7561.0, 500.0, 10000.0, 400.0, 0.002
// LMR_NOISY, int, 6.0, 0.0, 20.0, 1.0, 0.002
// HIST_BONUS_COEF, int, 168.0, 50.0, 500.0, 15.0, 0.002
// HIST_BONUS_BIAS, int, -59.0, -500.0, 500.0, 10.0, 0.002
// HIST_BONUS_MAX, int, 1530.0, 1000.0, 2500.0, 150.0, 0.002
// HIST_EVAL_BONUS, int, 145.0, 50.0, 500.0, 15.0, 0.002
// CORRHIST_BONUS_MAX, int, 512.0, 64.0, 1024.0, 48.0, 0.002
// CORRHIST_BONUS_SCALE, float, 8.0, 1.0, 16.0, 0.8, 0.002
// ASP_DELTA, int, 38.0, 32.0, 64.0, 3, 0.002
// ASP_GROWTH, float, 1.3, 1.1, 4.0, 0.15, 0.002
// OPTIMISM_MAX, int, 64.0, 16.0, 512.0, 8.0, 0.002
// VALUE_P, int, 111.0, 50.0, 200.0, 10.0, 0.002
// VALUE_N, int, 312.0, 200.0, 400.0, 20.0, 0.002
// VALUE_B, int, 290.0, 200.0, 400.0, 20.0, 0.002
// VALUE_R, int, 513.0, 400.0, 800.0, 40.0, 0.002
// VALUE_Q, int, 934.0, 800.0, 1600.0, 80.0, 0.002