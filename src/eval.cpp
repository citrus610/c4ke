#include "chess.cpp"

#define S(MG, EG) (MG + (EG << 16))

i32 PHASE[] { 0, 1, 1, 2, 4, 0 },
    VALUE[] { 110, 319, 294, 518, 912, 2000, 0 },
    LAYOUT[] { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK },
    MATERIAL[] { S(95, 206), S(360, 586), S(375, 595), S(506, 1062), S(1271, 1795), 0 };

#define SCALE 8

#define BISHOP_PAIR S(30, 96)
#define KING_OPEN S(-69, -7)
#define KING_SEMIOPEN S(-33, 13)
#define KING_PAWN_THREAT S(14, 53)
#define ROOK_OPEN S(25, -5)
#define ROOK_SEMIOPEN S(15, 19)
#define PAWN_PROTECTED S(23, 28)
#define PAWN_DOUBLED S(10, 39)
#define PAWN_SHIELD S(29, -13)
#define PASSER_BLOCKED S(-10, 72)

#define TEMPO 20

#define DATA_STR "0-,-/670,/13684 /12233/'0//.1200121001.--5+#&343%&&()))%\"%')))('%'('(')'%%&()()&%%%&'(*)(,' $!+)21,+ '&%' ( !#&32  #!)) % (E* #'&)*.* 1,*)(%$'$\"#$)6'#&(+,(%#%&(((''($$$'((+* !&)+++&#%(****!))&%&'(&$')))(&#&''((('%(()'&'&&%&))*(&% %)+**& ')(,  $,4@@ \"$,DD*.& 9)C OZ N/2+($\" !) %)-032"

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

#define OFFSET_MOBILITY S(-9, -2)
#define OFFSET_PASSER S(-5, -3)
#define OFFSET_PHALANX S(1, 1)
#define OFFSET_THREAT S(10, -7)
#define OFFSET_PUSH_THREAT S(20, -16)
#define OFFSET_KING_ATTACK S(8, -27)
#define OFFSET_PST S(-23, -14)
#define OFFSET_KING_PASSER S(-21, -16)

i32 get_data(i32 index) {
    auto data = DATA_STR;

    return data[index] + (data[index + INDEX_EG] << 16) - S(32, 32);
}