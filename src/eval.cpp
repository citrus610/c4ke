#include "chess.cpp"

#define S(MG, EG) (MG + (EG << 16))

i32 PHASE[] { 0, 1, 1, 2, 4, 0 },
    VALUE[] { 110, 319, 294, 518, 912, 2000, 0 },
    LAYOUT[] { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK },
    MATERIAL[] { S(95, 196), S(362, 575), S(378, 583), S(504, 1050), S(1294, 1716), 0 };

#define SCALE 8

#define BISHOP_PAIR S(29, 95)
#define KING_OPEN S(-69, -6)
#define KING_SEMIOPEN S(-33, 12)
#define KING_PAWN_THREAT S(19, 50)
#define ROOK_OPEN S(25, -5)
#define ROOK_SEMIOPEN S(16, 17)
#define PAWN_PROTECTED S(23, 27)
#define PAWN_DOUBLED S(10, 39)
#define PAWN_SHIELD S(29, -14)
#define PASSER_BLOCKED S(-7, 66)

#define TEMPO 20

#define DATA_STR "0-,-/570,/13684 /12233/'1//.120/121001.--5+$&343$&&()))%\"%'))*('%'('(')'%%&()()&%%&''(*)(,' $!+)21,* %$#% & !#'32  #!** ' ,7' !%$'(+' ,*(''$$&$\"#$)6&\"$'*+'$#$%'''&&'#\"#&''*)  %(***&\"$')))) ))&%&'(&%')))(&#&''((('%(()'&'&&%&())(&% %)+**& (*)/  $,4?? \"$+BA+/( <0J '* A-1*'$! \"' $(,.1/"

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

#define OFFSET_MOBILITY S(-9, -3)
#define OFFSET_PASSER S(-3, -3)
#define OFFSET_PHALANX S(1, 1)
#define OFFSET_THREAT S(10, -8)
#define OFFSET_PUSH_THREAT S(19, -22)
#define OFFSET_KING_ATTACK S(11, -14)
#define OFFSET_PST S(-23, -13)
#define OFFSET_KING_PASSER S(-17, -15)

i32 get_data(i32 index) {
    auto data = DATA_STR;

    return data[index] + (data[index + INDEX_EG] << 16) - S(32, 32);
}