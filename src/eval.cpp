#include "chess.cpp"

#define S(MG, EG) (MG + (EG << 16))

i32 PHASE[] { 0, 1, 1, 2, 4, 0 },
    VALUE[] { 100, 320, 330, 500, 900, 2000, 0 },
    LAYOUT[] { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK },
    MATERIAL[] { S(107, 148), S(415, 470), S(425, 484), S(572, 838), S(1156, 1426) };

#define SCALE 8

#define BISHOP_PAIR S(39, 74)
#define KING_OPEN S(-37, -25)
#define KING_SEMIOPEN S(-25, 23)
#define ROOK_OPEN S(16, -19)
#define ROOK_SEMIOPEN S(16, 22)
#define PAWN_PROTECTED S(18, 30)
#define PAWN_DOUBLED S(24, 37)
#define PAWN_SHIELD S(24, -7)

#define TEMPO 20

#define DATA_STR "0.-/1540.013383 /22223/'/-.-/335021/10./-/((*5EC#&&''((##&&(('&%%&&&&%'&#%'()'&%%%&''''%'(# \"\")).-*( ! !#$% !\"$45..* $&% ,,*)+/5,%(,/1-+.(),-.--.'(*-///-#$*0/12. (+.253,))(''()(#'*++*(%&)))))(%)))('(('&(()()() )+,*+( +./6  #&(-4 !\"(6<R\\M  '%?"

#define INDEX_EG 121

#define INDEX_PST_RANK 0
#define INDEX_PST_FILE 48
#define INDEX_MOBILITY 95
#define INDEX_PASSER 100
#define INDEX_PHALANX 106
#define INDEX_THREAT 112
#define INDEX_KING_ATTACK 116

#define OFFSET_MOBILITY S(-5, -12)
#define OFFSET_PASSER S(-3, 1)
#define OFFSET_PHALANX S(1, 3)
#define OFFSET_THREAT S(51, 9)
#define OFFSET_KING_ATTACK S(9, -11)
#define OFFSET_PST S(-22, -20)

int get_data(int index) {
    auto data = DATA_STR;

    return data[index] + data[index + INDEX_EG] * 0x10000 - S(32, 32);
}