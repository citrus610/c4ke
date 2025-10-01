#include "chess.cpp"

#define S(MG, EG) (MG + (EG << 16))

i32 PHASE[] { 0, 1, 1, 2, 4, 0 },
    VALUE[] { 110, 319, 294, 518, 912, 2000, 0 },
    LAYOUT[] { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK },
    MATERIAL[] { S(104, 175), S(358, 538), S(374, 550), S(504, 989), S(1272, 1552), 0 };

#define SCALE 8

#define BISHOP_PAIR S(29, 96)
#define KING_OPEN S(-70, -12)
#define KING_SEMIOPEN S(-34, 22)
#define ROOK_OPEN S(24, -2)
#define ROOK_SEMIOPEN S(17, 15)
#define PAWN_PROTECTED S(24, 26)
#define PAWN_DOUBLED S(12, 38)
#define PAWN_SHIELD S(29, -13)
#define PASSER_BLOCKED S(2, 84)

#define TEMPO 20

#define DATA_STR "0-+-/660-/13485 022223/'1//.130/121002.+.5,%&122%'')***&#&'***)(&((()()(&&')*)*'&&'(()**)-' $\",+21+)    #!# !#'31 !#\" ,5&(&%%&*5($&(,-)'%&&())((*%$%()),+\"#'),*+* %)-00. ,+)'')*(')+++)(&())***)'*++)()(('(**+*)) ',.--( --,6  #(-69 !#+==+.' #( @"

#define INDEX_EG 121

#define INDEX_PST_RANK 0
#define INDEX_PST_FILE 48
#define INDEX_MOBILITY 95
#define INDEX_PASSER 100
#define INDEX_PHALANX 106
#define INDEX_THREAT 112
#define INDEX_KING_ATTACK 116

#define OFFSET_MOBILITY S(-9, -6)
#define OFFSET_PASSER S(-1, 0)
#define OFFSET_PHALANX S(1, 1)
#define OFFSET_THREAT S(9, -8)
#define OFFSET_KING_ATTACK S(12, -14)
#define OFFSET_PST S(-24, -17)

i32 get_data(i32 index) {
    auto data = DATA_STR;

    return data[index] + data[index + INDEX_EG] * 0x10000 - S(32, 32);
}