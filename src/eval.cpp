#include "chess.cpp"

// data: lichess-big3 - loss: 0.074581

#define S(MG, EG) (MG + (EG << 16))

i32 PHASE[] { 0, 1, 1, 2, 4, 0 },
    LAYOUT[] { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK },
    VALUE[] { 121, 317, 300, 496, 1024, 5000, 0 },
    MATERIAL[] { S(90, 240), S(348, 683), S(362, 689), S(487, 1244), S(1137, 2234), 0 };

#define SCALE 8

#define BISHOP_PAIR S(30, 106)
#define KING_OPEN S(-66, -14)
#define KING_SEMIOPEN S(-34, 16)
#define ROOK_OPEN S(26, -10)
#define ROOK_SEMIOPEN S(13, 28)
#define PAWN_PROTECTED S(23, 31)
#define PAWN_DOUBLED S(9, 45)
#define PAWN_SHIELD S(29, -16)
#define MINOR_BEHIND_PAWN S(7, 39)

#define TEMPO 20

#define DATA_STR "0-+-/660,/13685 /12233/'1//.1200121001.--5,$&243&'')***&#&(**+)(&()()(*(&&')*)*'&&&'()+*)-( %\",*10+* '&%& ' !#&/4 !$\")( # (D, %)(*,0, 2-+**&%(%#$%*7($&),-)&$&&)))((*$$%(**,+  &+.--'!%(+./-!++('()*(&(+,,*(%())++*(&*++)()('&(+,-*'% '+.--(!)-*-  $.7EE \"$/PS*.& 1 B!N^ N04-)$\" ! $+158;:"

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
#define OFFSET_THREAT S(9, -6)
#define OFFSET_PUSH_THREAT S(19, -10)
#define OFFSET_KING_ATTACK S(7, -26)
#define OFFSET_PST S(-24, -17)
#define OFFSET_PASSER S(-29, -28)

i32 get_data(i32 index) {
    auto data = DATA_STR;

    return data[index] + (data[index + INDEX_EG] - 32 << 16) - 32;
}