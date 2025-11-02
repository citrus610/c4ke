#include "chess.cpp"

#define S(MG, EG) (MG + (EG << 16))

i32 PHASE[] { 0, 1, 1, 2, 4, 0 },
    VALUE[] { 110, 319, 294, 518, 912, 2000, 0 },
    LAYOUT[] { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK },
    MATERIAL[] { S(106, 202), S(361, 577), S(378, 587), S(507, 1051), S(1308, 1728), 0 };

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

#define DATA_STR "\"%%&(+-' #$%&%)&\"$%'))((#&'(+,+*()./162//22997.(-/03485 022223/'1//.02001210/2.-/4+#%234$&(***)(&((()(*(&&')*)*(&''(()**),' $\",+0/*( !! # # \"#'42 !#\" 06( \"%$'(+' 0/--.,+$%$#&%$\"\"\"!##$#!##! !\"$\"&%\"!!#&%**$!\"&-,853/19=@#%(+,(&$%%(((''($$$'((+* !&),*+'!$'*--,!%&)))(&#&''(((&%(()'&'%%$&()*(&% %)+)*& ()(-  $,4@; \"$,CB+.' $' @+/)&#! \"$ &*.131"

#define INDEX_EG 169

#define INDEX_PST_PAWN -8
#define INDEX_PST_RANK 40
#define INDEX_PST_FILE 80
#define INDEX_MOBILITY 127
#define INDEX_PASSER 132
#define INDEX_PHALANX 138
#define INDEX_THREAT 144
#define INDEX_KING_ATTACK 148
#define INDEX_KING_PASSER_US 153
#define INDEX_KING_PASSER_THEM 161

#define OFFSET_PST_PAWN S(-10, -7)
#define OFFSET_MOBILITY S(-7, -2)
#define OFFSET_PASSER S(-1, -3)
#define OFFSET_PHALANX S(1, 1)
#define OFFSET_THREAT S(9, -7)
#define OFFSET_KING_ATTACK S(9, -14)
#define OFFSET_PST S(-24, -14)
#define OFFSET_KING_PASSER S(-22, -16)

i32 get_data(i32 index) {
    auto data = DATA_STR;

    return data[index] + (data[index + INDEX_EG] << 16) - S(32, 32);
}