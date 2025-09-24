#pragma once

#include "move.h"

namespace bitboard
{

alignas(64) constexpr u64 RANK[8] = {
    0x00000000000000FFULL,
    0x000000000000FF00ULL,
    0x0000000000FF0000ULL,
    0x00000000FF000000ULL,
    0x000000FF00000000ULL,
    0x0000FF0000000000ULL,
    0x00FF000000000000ULL,
    0xFF00000000000000ULL
};

alignas(64) constexpr u64 FILE[8] = {
    0x0101010101010101ULL,
    0x0202020202020202ULL,
    0x0404040404040404ULL,
    0x0808080808080808ULL,
    0x1010101010101010ULL,
    0x2020202020202020ULL,
    0x4040404040404040ULL,
    0x8080808080808080ULL
};

constexpr u64 RANK_1 = bitboard::RANK[rank::RANK_1];
constexpr u64 RANK_2 = bitboard::RANK[rank::RANK_2];
constexpr u64 RANK_3 = bitboard::RANK[rank::RANK_3];
constexpr u64 RANK_4 = bitboard::RANK[rank::RANK_4];
constexpr u64 RANK_5 = bitboard::RANK[rank::RANK_5];
constexpr u64 RANK_6 = bitboard::RANK[rank::RANK_6];
constexpr u64 RANK_7 = bitboard::RANK[rank::RANK_7];
constexpr u64 RANK_8 = bitboard::RANK[rank::RANK_8];

constexpr u64 FILE_A = bitboard::FILE[file::FILE_A];
constexpr u64 FILE_B = bitboard::FILE[file::FILE_B];
constexpr u64 FILE_C = bitboard::FILE[file::FILE_C];
constexpr u64 FILE_D = bitboard::FILE[file::FILE_D];
constexpr u64 FILE_E = bitboard::FILE[file::FILE_E];
constexpr u64 FILE_F = bitboard::FILE[file::FILE_F];
constexpr u64 FILE_G = bitboard::FILE[file::FILE_G];
constexpr u64 FILE_H = bitboard::FILE[file::FILE_H];

constexpr u64 PRE_PROMOTION_RANK[2] = {
    bitboard::RANK_7,
    bitboard::RANK_2
};

constexpr u64 PROMOTION_RANK[2] = {
    bitboard::RANK_8,
    bitboard::RANK_1
};

constexpr u64 LIGHT = 0x55AA55AA55AA55AAULL;
constexpr u64 DARK = 0xAA55AA55AA55AA55ULL;

void init();

constexpr u64 create(i8 square)
{
    assert(square::is_valid(square));

    return 1ULL << square;
};

constexpr bool is_many(u64 bitboard)
{
    return bitboard & (bitboard - 1);
};

constexpr bool is_only(u64 bitboard)
{
    return bitboard && !bitboard::is_many(bitboard);
};

constexpr bool is_set(u64 bitboard, i8 square)
{
    assert(square::is_valid(square));

    return bitboard & (1ULL << square);
};

constexpr u64 get_set_bit(u64 bitboard, i8 square)
{
    assert(square::is_valid(square));

    return bitboard | (1ULL << square);
};

constexpr u64 get_pop_bit(u64 bitboard, i8 square)
{
    assert(square::is_valid(square));

    return bitboard ^ (1ULL << square);
};

constexpr i32 get_count(u64 bitboard)
{
    return __builtin_popcountll(bitboard);
};

constexpr i8 get_lsb(u64 bitboard)
{
    assert(bitboard);

    return __builtin_ctzll(bitboard);
};

constexpr i8 get_msb(u64 bitboard)
{
    assert(bitboard);

    return __builtin_clzll(bitboard) ^ 63;
};

constexpr u64 get_flip(u64 bitboard)
{
    return __builtin_bswap64(bitboard);
};

constexpr u64 get_pop_lsb(u64 bitboard)
{
    return bitboard & (bitboard - 1);
};

constexpr u64 get_pop_msb(u64 bitboard)
{
    return bitboard ^ (1Ull << bitboard::get_msb(bitboard));
};

constexpr u64 get_fill_up(u64 bitboard)
{
    bitboard |= bitboard << 8;
    bitboard |= bitboard << 16;
    bitboard |= bitboard << 32;

    return bitboard;
};

constexpr u64 get_fill_down(u64 bitboard)
{
    bitboard |= bitboard >> 8;
    bitboard |= bitboard >> 16;
    bitboard |= bitboard >> 32;

    return bitboard;
};

template <i8 COLOR>
constexpr u64 get_fill_up_relative(u64 bitboard)
{
    if constexpr (COLOR == color::BLACK) {
        return get_fill_down(bitboard);
    }

    get_fill_up(bitboard);
};

template <i8 COLOR>
constexpr u64 get_fill_down_relative(u64 bitboard)
{
    if constexpr (COLOR == color::BLACK) {
        return get_fill_up(bitboard);
    }

    get_fill_down(bitboard);
};

template <i8 DIRECTION>
constexpr u64 get_shift(u64 bitboard)
{
    switch (DIRECTION)
    {
    case direction::NORTH:
        return bitboard << 8;
    case direction::SOUTH:
        return bitboard >> 8;
    case direction::EAST:
        return (bitboard & ~bitboard::FILE_H) << 1;
    case direction::WEST:
        return (bitboard & ~bitboard::FILE_A) >> 1;
    case direction::NORTH_EAST:
        return (bitboard & ~bitboard::FILE_H) << 9;
    case direction::NORTH_WEST:
        return (bitboard & ~bitboard::FILE_A) << 7;
    case direction::SOUTH_EAST:
        return (bitboard & ~bitboard::FILE_H) >> 7;
    case direction::SOUTH_WEST:
        return (bitboard & ~bitboard::FILE_A) >> 9;
    };

    assert(false);

    return 0;
};

u64 get_between(i8 square_1, i8 square_2);

u64 get_line(i8 square_1, i8 square_2);

constexpr i8 pop_lsb(u64& bitboard)
{
    i8 lsb = bitboard::get_lsb(bitboard);

    bitboard &= bitboard - 1;

    return lsb;
};

constexpr i8 pop_msb(u64& bitboard)
{
    i8 msb = bitboard::get_msb(bitboard);

    bitboard ^= (1Ull << msb);

    return msb;
};

inline void print(u64 bitboard) {

    for (i32 rank = 7; rank >= 0; rank--) {
        char line[] = ". . . . . . . .";

        for (i32 file = 0; file < 8; file++) {
            if (bitboard::is_set(bitboard, square::create(file, rank))) {
                line[2 * file] = 'X';
            }
        }

        printf("%s\n", line);
    }

    printf("\n");
}

};