#pragma once

#include "piece.h"
#include "square.h"

namespace move
{

constexpr usize MAX = 256;

constexpr u16 NONE = 0;

namespace type
{

constexpr u16 NORMAL = 0;
constexpr u16 PROMOTION = 1 << 14;
constexpr u16 ENPASSANT = 2 << 14;
constexpr u16 CASTLING = 3 << 14;

};

constexpr u16 create(i8 square_from, i8 square_to)
{
    assert(square::is_valid(square_from));
    assert(square::is_valid(square_to));
    
    return (square_from << 6) + square_to;
};

template <u16 T = type::NORMAL>
constexpr u16 get(i8 square_from, i8 square_to, i8 promotion_type = piece::type::KNIGHT)
{
    assert(square::is_valid(square_from));
    assert(square::is_valid(square_to));
    assert(promotion_type >= piece::type::KNIGHT && promotion_type <= piece::type::QUEEN);
    
    return T + ((promotion_type - piece::type::KNIGHT) << 12) + (square_from << 6) + square_to;
};

constexpr i8 get_from(u16 move)
{
    return (move >> 6) & 0x3F;
};

constexpr i8 get_to(u16 move)
{
    return move & 0x3F;
};

constexpr u16 get_type(u16 move)
{
    return move & (3 << 14);
};

constexpr i8 get_promotion_type(u16 move)
{
    return i8((move >> 12) & 3) + piece::type::KNIGHT;
};

constexpr std::string get_str(u16 move)
{
    i8 from = move::get_from(move);
    i8 to = move::get_to(move);

    if (move::get_type(move) == move::type::CASTLING) {
        switch (to)
        {
        case square::A1:
            to = square::C1;
            break;
        case square::H1:
            to = square::G1;
            break;
        case square::A8:
            to = square::C8;
            break;
        case square::H8:
            to = square::G8;
            break;
        }
    }

    auto from_str = square::get_str(from);
    auto to_str = square::get_str(to);

    auto str = from_str + to_str;

    if (move::get_type(move) == move::type::PROMOTION) {
        i32 promotion_type = move::get_promotion_type(move);

        str.push_back(piece::type::get_char(promotion_type));
    }

    return str;
};

};