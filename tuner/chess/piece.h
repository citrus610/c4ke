#pragma once

#include "color.h"

namespace piece::type
{

enum : i8
{
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NONE = -1
};

constexpr i8 create(char c)
{
    if (c == 'P' || c == 'p') {
        return piece::type::PAWN;
    }
    else if (c == 'N' || c == 'n') {
        return piece::type::KNIGHT;
    }
    else if (c == 'B' || c == 'b') {
        return piece::type::BISHOP;
    }
    else if (c == 'R' || c == 'r') {
        return piece::type::ROOK;
    }
    else if (c == 'Q' || c == 'q') {
        return piece::type::QUEEN;
    }
    else if (c == 'K' || c == 'k') {
        return piece::type::KING;
    }

    return piece::type::NONE;
};

constexpr bool is_valid(i8 piece_type)
{
    return piece_type >= piece::type::PAWN && piece_type <= piece::type::KING;
};

constexpr char get_char(i8 piece_type)
{
    assert(piece::type::is_valid(piece_type));

    return
        piece_type == piece::type::PAWN ? 'p' :
        piece_type == piece::type::KNIGHT ? 'n' :
        piece_type == piece::type::BISHOP ? 'b' :
        piece_type == piece::type::ROOK ? 'r' :
        piece_type == piece::type::QUEEN ? 'q' : 'k';
};

};

namespace piece
{

enum : i8
{
    WHITE_PAWN,
    BLACK_PAWN,
    WHITE_KNIGHT,
    BLACK_KNIGHT,
    WHITE_BISHOP,
    BLACK_BISHOP,
    WHITE_ROOK,
    BLACK_ROOK,
    WHITE_QUEEN,
    BLACK_QUEEN,
    WHITE_KING,
    BLACK_KING,
    NONE = -1
};

constexpr i8 create(i8 piece_type, i8 color)
{
    assert(piece::type::is_valid(piece_type));
    assert(color::is_valid(color));
    
    return piece_type * 2 + color;
};

constexpr i8 create(char c)
{
    switch (c)
    {
    case 'P':
        return piece::WHITE_PAWN;
    case 'N':
        return piece::WHITE_KNIGHT;
    case 'B':
        return piece::WHITE_BISHOP;
    case 'R':
        return piece::WHITE_ROOK;
    case 'Q':
        return piece::WHITE_QUEEN;
    case 'K':
        return piece::WHITE_KING;
    case 'p':
        return piece::BLACK_PAWN;
    case 'n':
        return piece::BLACK_KNIGHT;
    case 'b':
        return piece::BLACK_BISHOP;
    case 'r':
        return piece::BLACK_ROOK;
    case 'q':
        return piece::BLACK_QUEEN;
    case 'k':
        return piece::BLACK_KING;
    }

    return piece::NONE;
};

constexpr bool is_valid(i8 piece)
{
    return piece >= piece::WHITE_PAWN && piece <= piece::BLACK_KING;
};

constexpr i8 get_type(i8 piece)
{
    assert(piece::is_valid(piece));

    return piece / 2;
};

constexpr i8 get_color(i8 piece)
{
    assert(piece::is_valid(piece));
    
    return piece & 1;
};

constexpr char get_char(i8 piece)
{
    if (!piece::is_valid(piece)) {
        return '.';
    }

    const char piece_str[] = {
        'P', 'p',
        'N', 'n',
        'B', 'b',
        'R', 'r',
        'Q', 'q',
        'K', 'k'
    };

    return piece_str[piece];
};

};