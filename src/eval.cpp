#include "chess.cpp"

#define S(MG, EG) (MG + (EG << 16))

i32 PHASE[] { 0, 1, 1, 2, 4, 0 },
    VALUE[] { 100, 320, 330, 500, 900, 2000, 0 },
    LAYOUT[] { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK },
    MATERIAL[] { S(80, 100), S(200, 300), S(250, 350), S(400, 600), S(800, 1000), 0 };

#define SCALE 8

#define BISHOP_PAIR S(29, 95)
#define KING_OPEN S(-69, -13)
#define KING_SEMIOPEN S(-32, 20)
#define ROOK_OPEN S(24, -3)
#define ROOK_SEMIOPEN S(18, 14)
#define PAWN_PROTECTED S(24, 28)
#define PAWN_DOUBLED S(10, 38)
#define PAWN_SHIELD S(35, -15)
#define PASSER_BLOCKED S(1, 82)

#define TEMPO 20

#define DATA_STR "%'&'))*$#$%%'$&$%&'())('&((),,*)++/1482.342:64$ 3569:>:&3555562*53325743KKJIHKGD)2( \"-...0244432.1101020++,-/-/-DEEEEFGG(,& $!+*43-+ !  $\"% \"#'42 !#\" .6'''&$''&$%%$$$%$#&'#\"\"#%#*(%\"\"#'&./'  #+,:76,-/9=457;<86433556546>>?ABCEDEFKMONON!$)-00- 579::87555577653ABBA@A??JLMNOOMM &+.,,' **)3  \"',87 #%,AC,/( #& ?"

#define INDEX_EG 153

#define INDEX_PST_PAWN 0
#define INDEX_PST_RANK 40
#define INDEX_PST_FILE 80
#define INDEX_MOBILITY 127
#define INDEX_PASSER 132
#define INDEX_PHALANX 138
#define INDEX_THREAT 144
#define INDEX_KING_ATTACK 148

#define OFFSET_PST_PAWN S(-7, 2)
#define OFFSET_MOBILITY S(-11, -3)
#define OFFSET_PASSER S(-2, 1)
#define OFFSET_PHALANX S(1, 0)
#define OFFSET_THREAT S(9, -8)
#define OFFSET_KING_ATTACK S(11, -14)
#define OFFSET_PST S(-20, -16)

int get_data(int index) {
    auto data = DATA_STR;

    return data[index] + data[index + INDEX_EG] * 0x10000 - S(32, 32);
}