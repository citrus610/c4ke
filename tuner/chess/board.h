#pragma once

#include "attack.h"
#include "bitboard.h"

class Board
{
public:
    u64 pieces[6];
    u64 colors[2];
    i8 board[64];
    i8 stm;
public:
    Board();
public:
    void set(const std::string& fen);
    void print();
};