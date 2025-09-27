#include "chess.cpp"

#define S(MG, EG) (MG + (EG << 16))

i32 PHASE[] { 0, 1, 1, 2, 4, 0 },
    VALUE[] { 100, 320, 330, 500, 900, 2000, 0 },
    LAYOUT[] { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK },
    MATERIAL[] { S(80, 100), S(200, 300), S(250, 350), S(400, 600), S(800, 1000), 0 };

#define SCALE 8

#define BISHOP_PAIR S(32, 85)
#define KING_OPEN S(-65, -21)
#define KING_SEMIOPEN S(-35, 30)
#define ROOK_OPEN S(25, -9)
#define ROOK_SEMIOPEN S(16, 21)
#define PAWN_PROTECTED S(23, 24)
#define PAWN_DOUBLED S(11, 37)
#define PAWN_SHIELD S(28, -13)
#define PASSER_BLOCKED S(5, 50)

#define TEMPO 20

#define DATA_STR "+*(*,31+13579<9%1343451(32213532>?==<>:7(0' \"--.&((*+++'-0134421-00/0/1/,+,././-99::;;==),' $\"+*21+( !  #!$ !#'11 '9+ ,3((+**+/6(01256211//111002777:;;=<57;=??@>!%),/0- .-+))+,+124443201112221/9::97877<<=>?\?>> '+.,,' 1..?  \"'+37 \"$,;;fzN #' ;"

#define INDEX_EG 121

#define INDEX_PST_RANK 0
#define INDEX_PST_FILE 48
#define INDEX_MOBILITY 95
#define INDEX_PASSER 100
#define INDEX_PHALANX 106
#define INDEX_THREAT 112
#define INDEX_KING_ATTACK 116

#define OFFSET_MOBILITY S(-9, -7)
#define OFFSET_PASSER S(-2, 0)
#define OFFSET_PHALANX S(1, 0)
#define OFFSET_THREAT S(70, -45)
#define OFFSET_KING_ATTACK S(11, -12)
#define OFFSET_PST S(-19, -16)

int get_data(int index) {
    auto data = DATA_STR;

    return data[index] + data[index + INDEX_EG] * 0x10000 - S(32, 32);
}