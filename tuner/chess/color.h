#pragma once

#include "types.h"

namespace color
{

enum : i8
{
    WHITE,
    BLACK,
    NONE = -1
};

constexpr i8 create(char c)
{
    return c == 'w' ? color::WHITE : c == 'b' ? color::BLACK : color::NONE;
};

constexpr bool is_valid(i8 color)
{
    return color == color::WHITE || color == color::BLACK;
};

constexpr char get_char(i8 color)
{
    assert(color::is_valid(color));
    
    return color == color::WHITE ? 'w' : 'b';
};

};