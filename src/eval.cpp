#include "chess.cpp"

#define S(MG, EG) (MG + (EG << 16))

i32 PHASE[] { 0, 1, 1, 2, 4, 0 },
    VALUE[] { 110, 319, 294, 518, 912, 2000, 0 },
    LAYOUT[] { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK },
    MATERIAL[] { S(97, 191), S(364, 567), S(379, 575), S(506, 1038), S(1295, 1696), 0 };

#define SCALE 8

#define BISHOP_PAIR S(29, 97)
#define KING_OPEN S(-70, -9)
#define KING_SEMIOPEN S(-34, 14)
#define ROOK_OPEN S(24, -5)
#define ROOK_SEMIOPEN S(17, 17)
#define PAWN_PROTECTED S(24, 28)
#define PAWN_DOUBLED S(13, 36)
#define PAWN_SHIELD S(29, -16)
#define PASSER_BLOCKED S(5, 60)

#define TEMPO 20

#define DATA_STR "0-+-/570,/13684 /12243/'1//.120/121001.-.5,$%133$&&()))%\"%'))*('%'('(')'%%&()()&%%&''(*)(,' $!+)10+) ##\"$ & !#&42  #!** ( +6' \"$#%'*& /-++*(('%$$%*6'#%(+,(%$%&((('')$#$'((+*!!&)*++'\"$'*--, **'&'()'&(***)'$'(()))(&))*('(''&')*+)'& &*,++'!*+*0  $,3>> #%-CB+/( =3M &) A+/*'#! \"$ &*.020"

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

#define OFFSET_MOBILITY S(-8, -4)
#define OFFSET_PASSER S(-2, -3)
#define OFFSET_PHALANX S(1, 0)
#define OFFSET_THREAT S(10, -8)
#define OFFSET_PUSH_THREAT S(19, -25)
#define OFFSET_KING_ATTACK S(12, -14)
#define OFFSET_PST S(-23, -15)
#define OFFSET_KING_PASSER S(-19, -16)

i32 get_data(i32 index) {
    auto data = DATA_STR;

    return data[index] + (data[index + INDEX_EG] << 16) - S(32, 32);
}