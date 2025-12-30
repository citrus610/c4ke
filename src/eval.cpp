#include "chess.cpp"

// data: lichess-big3 - loss: 0.074685

#define S(MG, EG) (MG + (EG << 16))

i32 PHASE[] { 0, 1, 1, 2, 4, 0 },
    VALUE[] { 111, 312, 290, 513, 934, 5000, 0 },
    LAYOUT[] { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK },
    MATERIAL[] { S(92, 231), S(359, 658), S(375, 667), S(505, 1197), S(1290, 1975), 0 };

#define SCALE 8

#define BISHOP_PAIR S(29, 106)
#define KING_OPEN S(-67, -14)
#define KING_SEMIOPEN S(-34, 17)
#define ROOK_OPEN S(26, -10)
#define ROOK_SEMIOPEN S(14, 27)
#define PAWN_PROTECTED S(23, 30)
#define PAWN_DOUBLED S(10, 44)
#define PAWN_SHIELD S(29, -16)

#define TEMPO 20

#define DATA_STR "0-+-/660,/13684 /12233/'0//.1201121001.--5+$'243$&&()))%\"%'))*('%'('(')'%%&()()&%%%&'(*)(,' $!+)10+) '&%& ' !#&33  #!)) $ (E) %)(+,0, 2-+**'%'$##$)6'#&(,,(%#%%((('&)##$())+*  &),+,&!$'*-., ++('()*(')+,,)(%())+**)'*++)()('&(++,*'& '+.--(!*-*1  %.7DE \"%.JI+/' 2&C Vd Y04,)$\" ! #*/46:9"

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
#define OFFSET_PHALANX S(1, 1)
#define OFFSET_THREAT S(10, -7)
#define OFFSET_PUSH_THREAT S(20, -11)
#define OFFSET_KING_ATTACK S(9, -36)
#define OFFSET_PST S(-23, -16)
#define OFFSET_PASSER S(-29, -27)

i32 get_data(i32 index) {
    auto data = DATA_STR;

    return data[index] + (data[index + INDEX_EG] << 16) - S(32, 32);
}