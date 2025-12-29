#include "chess.cpp"

// data: lichess-big3 - loss: 0.074685

#define S(MG, EG) (MG + (EG << 16))

i32 PHASE[] { 0, 1, 1, 2, 4, 0 },
    VALUE[] { 111, 312, 290, 513, 934, 5000, 0 },
    LAYOUT[] { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK },
    MATERIAL[] { S(92, 234), S(360, 655), S(375, 665), S(508, 1191), S(1297, 1956), 0 };

#define SCALE 8

#define BISHOP_PAIR S(29, 106)
#define KING_OPEN S(-69, -9)
#define KING_SEMIOPEN S(-33, 15)
#define KING_PAWN_THREAT S(11, 61)
#define ROOK_OPEN S(26, -10)
#define ROOK_SEMIOPEN S(14, 27)
#define PAWN_PROTECTED S(23, 31)
#define PAWN_DOUBLED S(10, 44)
#define PAWN_SHIELD S(29, -14)

#define TEMPO 20

#define DATA_STR "0-+-.660,/13684 /12233/'0//.1201121001.--5+$'453%&&()))%\"%'))*('%'('(')'%%&()()&%%%&'(*)(,' $!+)21,* '&%' ' !#&33  #!)) $ )F* %)(+,0, /+)('$#'$\"#$)7'#&(,,(%#%&((('&(##$())+*  &),+,&#%(**+*!**'&'()'&(*++('$'((*))(%)**('('&&'**+)&% &*-++'!(+(/  %.7EE \"$.HH+/' 5&C Ze Z25-)%\" \"! %*.154"

#define INDEX_EG 141

#define INDEX_PST_RANK 0
#define INDEX_PST_FILE 48
#define INDEX_MOBILITY 95
#define INDEX_PASSER 100
#define INDEX_PHALANX 106
#define INDEX_THREAT 112
#define INDEX_PUSH_THREAT 116
#define INDEX_KING_ATTACK 120
#define INDEX_KING_PASSER_US 125
#define INDEX_KING_PASSER_THEM 133

#define OFFSET_MOBILITY S(-9, -2)
#define OFFSET_PHALANX S(1, 1)
#define OFFSET_THREAT S(10, -7)
#define OFFSET_PUSH_THREAT S(20, -12)
#define OFFSET_KING_ATTACK S(8, -37)
#define OFFSET_PST S(-23, -15)
#define OFFSET_PASSER S(-27, -23)

i32 get_data(i32 index) {
    auto data = DATA_STR;

    return data[index] + (data[index + INDEX_EG] << 16) - S(32, 32);
}