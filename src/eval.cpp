#include "chess.cpp"

#define S(MG, EG) (MG + (EG << 16))

i32 PHASE[] { 0, 1, 1, 2, 4, 0 },
    VALUE[] { 110, 319, 294, 518, 912, 2000, 0 },
    LAYOUT[] { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK },
    MATERIAL[] { S(100, 192), S(356, 574), S(374, 586), S(501, 1050), S(1298, 1731), 0 };

#define SCALE 8

#define BISHOP_PAIR S(28, 97)
#define KING_OPEN S(-68, -10)
#define KING_SEMIOPEN S(-34, 14)
#define ROOK_OPEN S(25, -5)
#define ROOK_SEMIOPEN S(16, 17)
#define PAWN_PROTECTED S(24, 28)
#define PAWN_DOUBLED S(12, 37)
#define PAWN_SHIELD S(30, -15)
#define PASSER_BLOCKED S(1, 64)

#define TEMPO 20

#define DATA_STR "0-+-/680-/13485 022123/'1//.13001210/2.--5,$%133$&&()))%\"%&)))('%'''('('%%&()()&%%&''())(,' $!+)31,+ $#\"$ & !#'42 !#\" -7' \"%$'(+' 0/,++(('%$$%*6'#%(+,(&$%%(((''($#$'((+* !&),*+'\"$'*--, **(&'()'&(***)'%'(()))(&))*('(''&')**)'& &*,++'!*+*/  %-4?? \"$,CB*.' $) A,0*'#! !$ &*.031"

#define INDEX_EG 137

#define INDEX_PST_RANK 0
#define INDEX_PST_FILE 48
#define INDEX_MOBILITY 95
#define INDEX_PASSER 100
#define INDEX_PHALANX 106
#define INDEX_THREAT 112
#define INDEX_KING_ATTACK 116
#define INDEX_KING_PASSER_US 121
#define INDEX_KING_PASSER_THEM 129

#define OFFSET_MOBILITY S(-9, -4)
#define OFFSET_PASSER S(-2, -4)
#define OFFSET_PHALANX S(1, 1)
#define OFFSET_THREAT S(9, -7)
#define OFFSET_KING_ATTACK S(11, -14)
#define OFFSET_PST S(-23, -15)
#define OFFSET_KING_PASSER S(-21, -16)

i32 get_data(i32 index) {
    auto data = DATA_STR;

    return data[index] + (data[index + INDEX_EG] << 16) - S(32, 32);
}