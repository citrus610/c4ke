#include "bitboard.h"
#include "attack.h"

namespace bitboard
{

alignas(64) u64 BETWEEN[64][64];
alignas(64) u64 LINE[64][64];

void init()
{
    std::memset(BETWEEN, 0, sizeof(BETWEEN));
    std::memset(LINE, 0, sizeof(LINE));

    for (i8 sq1 = 0; sq1 < 64; ++sq1) {
        for (i8 sq2 = 0; sq2 < 64; ++sq2) {
            if (sq1 == sq2) {
                continue;
            }

            const u64 bb1 = bitboard::create(sq1);
            const u64 bb2 = bitboard::create(sq2);

            if (attack::get_rook(sq1, 0) & bb2) {
                BETWEEN[sq1][sq2] = attack::get_rook(sq1, bb2) & attack::get_rook(sq2, bb1);
                LINE[sq1][sq2] = (attack::get_rook(sq1, 0) & attack::get_rook(sq2, 0)) | bb1 | bb2;
            }

            if (attack::get_bishop(sq1, 0) & bb2) {
                BETWEEN[sq1][sq2] = attack::get_bishop(sq1, bb2) & attack::get_bishop(sq2, bb1);
                LINE[sq1][sq2] = (attack::get_bishop(sq1, 0) & attack::get_bishop(sq2, 0)) | bb1 | bb2;
            }
        }
    }
};

u64 get_between(i8 square_1, i8 square_2)
{
    assert(square::is_valid(square_1));
    assert(square::is_valid(square_2));

    return BETWEEN[square_1][square_2];
};

u64 get_line(i8 square_1, i8 square_2)
{
    assert(square::is_valid(square_1));
    assert(square::is_valid(square_2));

    return LINE[square_1][square_2];
};

};