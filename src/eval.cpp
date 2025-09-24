#include "chess.cpp"

#define S(MG, EG) (MG + (EG << 16))

i32 PHASE[] { 0, 1, 1, 2, 4, 0 },
    VALUE[] { 100, 320, 330, 500, 900, 2000, 0 },
    LAYOUT[] { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK },
    MATERIAL[] { S(73, 109), S(228, 307), S(237, 323), S(367, 687), S(677, 973) };

#define SCALE_PST 4
#define SCALE_PHALANX 4

#define BISHOP_PAIR S(10, 25)
#define KING_OPEN S(-11, -8)
#define KING_SEMI_OPEN S(-8, 8)
#define ROOK_OPEN S(5, -7)
#define ROOK_SEMI_OPEN S(5, 8)
#define PAWN_PROTECTED S(6, 10)
#define PAWN_DOUBLED S(7, 11)
#define PAWN_SHIELD S(7, -2)

#define TEMPO 20

#define DATA_STR "F?>@ACIF:;<>>A>28:99::83)'(()++,!\"\"!!!  EFBBDJX[ACCDCDDA688999879::::9::12344322  !!!!! IIFEFFJJ$### 3/177  !!#0H\"\"  !!\" YRQQRTVYCDGIKHGHDDFGGGGH$$%')()(  %*+,,*PUWXZ]ZTMMMLLMMM?BDDDDCA?@@AAA@?####\"##\" \"#$##!\"RXYZYYWR$%$\"  &.3BZ !!&-X'((  \" '"

#define INDEX_EG 121

#define INDEX_PST_RANK 0
#define INDEX_PST_FILE 48
#define INDEX_MOBILITY 95
#define INDEX_PASSER 100
#define INDEX_PHALANX 106
#define INDEX_THREAT 112
#define INDEX_KING_ATTACK 116

#define OFFSET_MOBILITY S(-1, -4)
#define OFFSET_PASSER S(-22, 2)
#define OFFSET_PHALANX S(2, 7)
#define OFFSET_THREAT S(28, 21)
#define OFFSET_KING_ATTACK S(3, -4)
#define OFFSET_PST S(-309, -443)

int get_data(int index) {
    auto data = DATA_STR;

    return data[index] + data[index + INDEX_EG] * 0x10000 - S(32, 32);
}