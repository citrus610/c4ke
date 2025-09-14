#include "chess.cpp"

#define S(MG, EG) (MG + (EG << 16))

int PHASE[] = { 0, 1, 1, 2, 4, 0 };
int VALUE[] = { 100, 320, 330, 500, 900, 2000, 0 };

// Values copied from 4ku, will replace later
int MATERIAL[] = { S(89, 147), S(350, 521), S(361, 521), S(479, 956), S(1046, 1782), 0 };

#define SCALE_PST 8
#define SCALE_PASSER 5
#define SCALE_PHALANX 8

#define BISHOP_PAIR S(25, 55)
#define KING_OPEN S(-75, 5)
#define KING_SEMI_OPEN S(-30, 15)
#define ROOK_OPEN S(25, 5)
#define ROOK_SEMI_OPEN S(10, 15)
#define PAWN_PROTECTED S(12, 16)
#define PAWN_DOUBLED S(12, 40)
#define PAWN_SHIELD S(30, -10)

#define TEMPO 20

#define DATA_STR "/,,.04//-/02341 /11111.%/.--/101121//.+-.0.+.444#\"#$%&&# #$&&&%#\"$%$%$&#\"#$%&%%#\"##$$%&%\"&#  \"&$%-,(( \"\"! $%' !\"#%*/ RRpk 4999++**+.++&(*./,+,)*+++++-(()+--., #(-002/%)+.0/-%&&%$%%%%\"$&((%$\"$%%&&&%$%&&%$%%$\"$%&''%$ $&'''$ !&(&#  \"$&*4>  !\"*/4 9R9      "

#define INDEX_EG 126

#define INDEX_PST_RANK 0
#define INDEX_PST_FILE 48
#define INDEX_MOBILITY 96
#define INDEX_PASSER 102
#define INDEX_PHALANX 109
#define INDEX_THREAT 116
#define INDEX_KING_ATTACK 121

#define OFFSET_PST_RANK S(-15, -11)
#define OFFSET_PST_FILE S(-4, -5)
#define OFFSET_MOBILITY S(-5, -1)
#define OFFSET_PASSER S(-2, 0)
#define OFFSET_PHALANX S(0, 0)
#define OFFSET_THREAT S(0, 0)
#define OFFSET_KING_ATTACK S(0, 0)

int get_data(int index) {
    auto data = DATA_STR;

    return data[index] + data[index + INDEX_EG] * 0x10000 - S(32, 32);
}