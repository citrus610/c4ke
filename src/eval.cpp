#include "chess.cpp"

// data: sirius 0.6 wdl - loss: 0.079084

#define S(MG, EG) (MG + (EG << 16))

i32 PHASE[] { 0, 1, 1, 2, 4, 0 },
    VALUE[] { 111, 312, 290, 513, 934, 5000, 0 },
    LAYOUT[] { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK },
    MATERIAL[] { S(80, 236), S(347, 605), S(357, 631), S(486, 1078), S(1236, 1823), 0 };

#define SCALE 8

#define BISHOP_PAIR S(29, 100)
#define KING_OPEN S(-49, -9)
#define KING_SEMIOPEN S(-25, 11)
#define KING_PAWN_THREAT S(14, 75)
#define ROOK_OPEN S(27, -18)
#define ROOK_SEMIOPEN S(17, 18)
#define PAWN_PROTECTED S(18, 32)
#define PAWN_DOUBLED S(13, 42)
#define PAWN_SHIELD S(24, -11)
#define PASSER_BLOCKED S(-3, 57)

#define TEMPO 20

#define DATA_STR "0,+-.470-.14583 .23333-(/,..0216010001.0/4+$&.73%&'(()(%#&')))(&%''''&(&$%')*)'%%&'''(('*+' #\"+)/.+' #\"!\" $  !$13  \" 02 / !,  &('()-* 2/-,+)(+(&'),;+&(+/0,*)((*+,,-.&'(,-.1,  (.222-&)-./0-%(('&''(&$')**''\"%'(((('$()(&%&'&$%&**('& %),**& (*)-  $,4?B #$+AE-/) 3(H AD E21+($\" \"- %)-/32"

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

#define OFFSET_MOBILITY S(-6, -3)
#define OFFSET_PHALANX S(1, 1)
#define OFFSET_THREAT S(9, -4)
#define OFFSET_PUSH_THREAT S(6, -14)
#define OFFSET_KING_ATTACK S(15, -8)
#define OFFSET_PST S(-23, -18)
#define OFFSET_PASSER S(-24, -19)

i32 get_data(i32 index) {
    auto data = DATA_STR;

    return data[index] + (data[index + INDEX_EG] << 16) - S(32, 32);
}