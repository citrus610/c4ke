#pragma once

#include "types.h"
#include "color.h"
#include "square.h"
#include "piece.h"
#include "move.h"
#include "bitboard.h"
#include "attack.h"
#include "board.h"

namespace chess
{

inline void init()
{
    attack::init();
    bitboard::init();
};

};