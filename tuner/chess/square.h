#pragma once

#include "color.h"

namespace direction
{

enum : i8
{
    NORTH = 8,
    WEST = -1,
    SOUTH = -8,
    EAST = 1,
    NORTH_EAST = 9,
    NORTH_WEST = 7,
    SOUTH_WEST = -9,
    SOUTH_EAST = -7
};

template <i8 COLOR>
constexpr i8 get(i8 direction)
{
    return COLOR == color::WHITE ? direction : -direction;
};

constexpr i8 get_relative(i8 direction, i8 color)
{
    assert(color::is_valid(color));
    
    return color == color::WHITE ? direction : -direction;
};

};

namespace file
{

enum : i8
{
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H,
    NONE = -1
};

constexpr i8 create(char c)
{
    if (c < 'a' || c > 'h') {
        return file::NONE;
    }

    return c - 'a';
};

constexpr bool is_valid(i8 file)
{
    return file >= file::FILE_A && file <= file::FILE_H;
};

constexpr char get_char(i8 file)
{
    assert(file::is_valid(file));

    return 'a' + file;
};

};

namespace rank
{

enum : i8
{
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
    NONE = -1
};

constexpr i8 create(char c)
{
    if (c < '1' || c > '8'){
        return rank::NONE;
    }

    return c - '1';
};

constexpr bool is_valid(i8 rank)
{
    return rank >= rank::RANK_1 && rank <= rank::RANK_8;
};

template <i8 COLOR>
constexpr i8 get(i8 rank)
{
    assert(rank::is_valid(rank));

    return COLOR == color::WHITE ? rank : rank::RANK_8 - rank;
};

constexpr i8 get_relative(i8 rank, i8 color)
{
    assert(rank::is_valid(rank));
    assert(color::is_valid(color));

    return color == color::WHITE ? rank : rank::RANK_8 - rank;
};

constexpr char get_char(i8 rank)
{
    assert(rank::is_valid(rank));

    return '1' + rank;
};

};

namespace square
{

enum : i8
{
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    NONE = -1
};

constexpr i8 create(i8 file, i8 rank)
{
    assert(file::is_valid(file));
    assert(rank::is_valid(rank));

    return file | (rank << 3);
};

constexpr bool is_valid(i8 square)
{
    return square >= square::A1 && square <= square::H8;
};

constexpr bool is_light(i8 square)
{
    assert(square::is_valid(square));

    return (square / 8 + square % 8) % 2 == 0;
};

constexpr bool is_dark(i8 square)
{
    assert(square::is_valid(square));

    return !square::is_light(square);
};

constexpr bool is_same_color(i8 square_1, i8 square_2)
{
    assert(square::is_valid(square_1));
    assert(square::is_valid(square_2));

    return ((9 * (square_1 ^ square_2)) & 8) == 0;
};

constexpr i8 get_file(i8 square)
{
    assert(square::is_valid(square));
    
    return square & 7;
};

constexpr i8 get_rank(i8 square)
{
    assert(square::is_valid(square));

    return square >> 3;
};

constexpr i8 get_flip_file(i8 square)
{
    assert(square::is_valid(square));

    return square ^ 7;
};

constexpr i8 get_flip_rank(i8 square)
{
    assert(square::is_valid(square));

    return square ^ 56;
};

constexpr i8 get_relative(i8 square, i8 color)
{
    assert(square::is_valid(square));
    assert(color::is_valid(color));

    return square ^ (color * 56);
};

constexpr i32 get_distance_file(i8 square_1, i8 square_2)
{
    assert(square::is_valid(square_1));
    assert(square::is_valid(square_2));

    return std::abs(square::get_file(square_1) - square::get_file(square_2));
};

constexpr i32 get_distance_rank(i8 square_1, i8 square_2)
{
    assert(square::is_valid(square_1));
    assert(square::is_valid(square_2));

    return std::abs(square::get_rank(square_1) - square::get_rank(square_2));
};

constexpr i32 get_distance(i8 square_1, i8 square_2)
{
    assert(square::is_valid(square_1));
    assert(square::is_valid(square_2));

    return square::get_distance_file(square_1, square_2) + square::get_distance_rank(square_1, square_2);
};

constexpr i32 get_chebyshev(i8 square_1, i8 square_2)
{
    assert(square::is_valid(square_1));
    assert(square::is_valid(square_2));

    return std::max(square::get_distance_file(square_1, square_2), square::get_distance_rank(square_1, square_2));
};

constexpr std::string get_str(i8 square)
{
    assert(square::is_valid(square));

    std::string result;

    result.push_back(file::get_char(square::get_file(square)));
    result.push_back(rank::get_char(square::get_rank(square)));

    return result;
};

};