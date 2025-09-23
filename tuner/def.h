#pragma once

#include "chess/chess.h"

using Pair = std::array<f64, 2>;

enum Phase
{
    MG,
    EG
};

constexpr i32 S(const i32 mg, const i32 eg)
{
    return static_cast<i32>(static_cast<u32>(eg) << 16) + mg;
};

constexpr i32 get_mg(i32 score)
{
    return static_cast<i16>(score);
};

constexpr i32 get_eg(i32 score)
{
    return static_cast<i16>((score + 0x8000) >> 16);
};