#include "chess.cpp"

#define S(MG, EG) (MG + (EG << 16))

#define TEMPO 20

int PHASE[] = { 0, 1, 1, 2, 4, 0 };
int VALUE[] = { 100, 320, 330, 500, 900, 2000, 0 };

int MATERIAL[] = { S(89, 147), S(350, 521), S(361, 521), S(479, 956), S(1046, 1782), 0 };

#define INDEX_ENDGAME 109
#define INDEX_PST_RANK 0
#define INDEX_PST_FILE 48
#define INDEX_MOBILITY 96
#define INDEX_PAWN_PASSED 102

int get_data(int index) {
    auto data = L"ȀǨǨǸȈȨȀȀǰȀȈȘȠȨȐƈȀȐȐȐȐȐǸưȀǸǰǰȀȐȈȐȐȘȐȀȀǸǠǰǸȈǸǠǸȨȨȨǸǰǸȀȈȐȐǸǠǸȀȐȐȐȈǸǰȀȈȀȈȀȐǸǰǸȀȈȐȈȈǸǰǸǸȀȀȈȐȈǰȐǸǠǠǰȐȀȀȈȇȃȃǻȀȀǻǶȊȏșȀȀǸǸȀȘȀȀǘǨǸȘȠȈȀȈǰǸȀȀȀȀȀȐǨǨǰȀȐȐȘȈƨǀǨȐȨȨȸȠǐǰȀȘȨȠȐǐȈȈȀǸȀȀȀȀǨǸȈȘȘȀǸǨǸȀȀȈȈȈȀǸȀȈȈȀǸȀȀǸǨǸȀȈȐȐȀǸǘǸȈȐȐȐǸǘȀȅȇȅȂǿȀȏȔȲɋɤʖ";

    return data[index] + (data[index + INDEX_ENDGAME] << 16) - S(512, 512);
}