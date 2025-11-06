#include "chess.cpp"

#define DATA_STR "MXTMMKKN4.*,-,&\"P]WD?JTPcOG6<5@LOHDK3rIORIS|MLLGFY[RNUO:VNUNJ5;F``bdhhg`LJHIJGLLojkilknqqo`MUtT:;[GE;I[^` HLI?:y:?@CEECFeefgggegFEGHGFEEeggggggf[OM0)'IU[otK1+=9 UUbYXu|7QRUOD13f`\\UPHC8GJKMMLIG63444448fJPJ?\\nFOLJ1Z)#.$l(d\\fOAMBHONNMM@=@FEFIITdf[RB>P]]\\VOJEDGB?A7QL\\TI= BOO[` BOIIU`WQRTOJIM8EDGKXd`PTZ\\[VSR! !!!   dSOQW^NLNMUTC=>1 GNY]ds=RFLOJIMONNOPSSTTMJRJRFTRTHb8k7c?@RILI&UNGVMvJNNLMM9NOIMNNR?APORUlllkkjihPQNMMMRPjnonoopkaXWsg|QG?4(?6\\^VrN3CH;3Feig[RULn*))-22.3GJJJJJHG !   !\" -''?!E@b^\\|M[LI+-j]]bu|YWGIPBM^?;>;:9@LMMOLJJOPM^^\\\\[]]^o9=QD\\M]UQUY|5:V]nO^NP9LJ?M\\YXaJEUTLKIF6VOSWVKNTHVRZRZRI_U[BQOSEEHFFdVLjMKNE>ZLWNRMGS]bivuutsrrsRQQQRRSR--++*,-.XBT_|Z8ZMF HX+>:#2Cpdn|Yy;|rB4P|r_Px||! ||||||e|  || !   \"5 N"

#define NNUE_INPUT 768
#define NNUE_INPUT_DECOMPOSED 56
#define NNUE_HIDDEN 8

#define NNUE_SCALE_L0 2116
#define NNUE_SCALE_L1 46
#define NNUE_SCALE_L0_L1 97336
#define NNUE_SCALE_EVAL 400

#define INDEX_L0_WEIGHT_R1 0
#define INDEX_L0_WEIGHT_R2 8
#define INDEX_L0_WEIGHT_F1 16
#define INDEX_L0_WEIGHT_F2 24
#define INDEX_L0_WEIGHT_P1 32
#define INDEX_L0_WEIGHT_P2 44
#define INDEX_L0_BIAS 672
#define INDEX_L1_WEIGHT 684
#define INDEX_L1_BIAS 708

i32 VALUE[] { 110, 319, 294, 518, 912, 2000, 0 },
    LAYOUT[] { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK },
    L0_W[NNUE_INPUT][NNUE_HIDDEN],
    L0_B[NNUE_HIDDEN],
    L1_W[2][NNUE_HIDDEN],
    L1_B;

i32 get_data(i32 index) {
    return DATA_STR[index] - 78;
}

i32 ft(i32 piece, i32 square, i32 stm) {
    return 384 * (piece % 2 != stm) + 64 * (piece / 2) + square ^ 56 * stm;
}

i32 screlu(i32 x) {
    x = clamp(x, 0, NNUE_SCALE_L0);

    return x * x;
}