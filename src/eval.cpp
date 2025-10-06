#include "chess.cpp"

#define S(MG, EG) (MG + (EG << 16))

i32 PHASE[] { 0, 1, 1, 2, 4, 0 },
    VALUE[] { 110, 319, 294, 518, 912, 2000, 0 },
    LAYOUT[] { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK },
    MATERIAL[] { S(106, 180), S(357, 566), S(374, 577), S(501, 1037), S(1307, 1668), 0 };

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

#define DATA_STR "0-+-/580-/13485 022223/'1//.130/1210/2.,.6,$$022%'')***&#&'***)(&((()()(&&'(*)*'&&'(()**)-( %\",*21,*    \" $ !#'52 !#\" ,6&!\"\" \"#%! +-**)()'%##%*6'#%'+,(&$%%((('')$#$'((+*!!&)+)*(\"$'*-., **(&'()'&(***)'%'(()))(&)**('(''&'))*)'' &*,++'!*+*1  $,3== \"$,@?+/' $) B*.)'#! \"\" %),.0-"

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
#define OFFSET_PASSER S(1, -3)
#define OFFSET_PHALANX S(1, 1)
#define OFFSET_THREAT S(9, -8)
#define OFFSET_KING_ATTACK S(12, -14)
#define OFFSET_KING_PASSER_US S(-2, -6)
#define OFFSET_KING_PASSER_THEM S(-12, -9)
#define OFFSET_PST S(-24, -15)

i32 get_data(i32 index) {
    auto data = DATA_STR;

    return data[index] + data[index + INDEX_EG] * 0x10000 - S(32, 32);
}