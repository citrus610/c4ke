#include <bits/stdc++.h>

using namespace std;

typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef uint64_t u64;

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
    int TT_BITS = 20;
    int TT_SHIFT = 64 - TT_BITS;
#else
    #define TT_BITS 20
    #define TT_SHIFT 44
#endif

#ifdef OB
    int THREADS = 1;
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
u64 KEYS[13][65]{};

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

u64 ray(u64 mask, u64 occupied, u64 (*func)(u64)) {
    mask = func(mask);

    mask |= func(mask & ~occupied);
    mask |= func(mask & ~occupied);
    mask |= func(mask & ~occupied);
    mask |= func(mask & ~occupied);
    mask |= func(mask & ~occupied);
    mask |= func(mask & ~occupied);

    return mask;
}

u64 knight(u64 mask, u64 occupied = 0) {
    return (mask << 6 | mask >> 10) & 0x3f3f3f3f3f3f3f3f | (mask << 10 | mask >> 6) & 0xfcfcfcfcfcfcfcfc | (mask << 17 | mask >> 15) & ~0x101010101010101 | (mask << 15 | mask >> 17) & ~0x8080808080808080;
}

u64 bishop(u64 mask, u64 occupied) {
    return ray(mask, occupied, nw) | ray(mask, occupied, ne) | ray(mask, occupied, sw) | ray(mask, occupied, se);
}

u64 rook(u64 mask, u64 occupied) {
    return ray(mask, occupied, north) | ray(mask, occupied, south) | ray(mask, occupied, west) | ray(mask, occupied, east);
}

u64 king(u64 mask, u64 occupied = 0) {
    return mask << 8 | mask >> 8 | (mask >> 1 | mask >> 9 | mask << 7) & ~0x8080808080808080 | (mask << 1 | mask << 9 | mask >> 7) & ~0x101010101010101;
}

// Shared states
struct TTEntry {
    u16 key, move;
    i16 score;
    u8 depth, bound;
};

TTEntry* TTABLE;
u16 BEST_MOVE;
u64 LIMIT_SOFT,
    LIMIT_HARD,
    VISITED[STACK_SIZE];
int VISITED_COUNT,
    RUNNING;

#ifdef OB
void print_bitboard(u64 bitboard) {

    for (int rank = 7; rank >= 0; rank--) {
        char line[] = ". . . . . . . .";

        for (int file = 0; file < 8; file++) {
            if (bitboard & 1ull << (file | (rank << 3))) {
                line[2 * file] = 'X';
            }
        }

        printf("%s\n", line);
    }

    printf("\n");
}
#endif