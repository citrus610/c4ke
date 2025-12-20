#include "search.cpp"

#ifdef OB
string move_str(i16 move) {
    string str;

    str += 97 + move_from(move) % 8;
    str += 49 + move_from(move) / 8;
    str += 97 + move_to(move) % 8;
    str += 49 + move_to(move) / 8;

    if (move_promo(move)) {
        str += 'q';
    }

    return str;
}

u64 perft(Board& board, i32 depth, bool is_root = FALSE) {
    if (depth <= 0) {
        return 1;
    }

    u64 nodes = 0;

    i16 moves[MAX_MOVE];
    i32 count = board.movegen(moves, TRUE);

    for (i32 i = 0; i < count; i++) {
        Board child = board;

        if (child.make(moves[i])) {
            continue;
        }

        u64 children = perft(child, depth - 1);

        nodes += children;

        if (is_root) {
            cout << move_str(moves[i]) << " - " << children << std::endl;
        }
    }

    return nodes;
}
#endif

#ifdef OB_MINI
void bench()
{
    // Fens from Iris
    string FENS[] = {
        "rnbqkb1r/1ppp1p1p/p3pnp1/8/2PP4/4P3/PPQ2PPP/RNB1KBNR w KQkq - 0 1",
        "r1bqkbnr/pp1ppp1p/6p1/2p5/2PnP3/2N5/PP1PNPPP/R1BQKB1R w KQkq - 0 1",
        "rn1qkb1r/p1pppp1p/bp3np1/8/2P2N2/3P4/PP2PPPP/RNBQKB1R w KQkq - 0 1",
        "r2qkbnr/ppp2ppp/2n5/P2ppb2/3P4/2P5/1P2PPPP/RNBQKBNR w KQkq - 0 1",
        "r2q1rk1/1ppbppbp/3p1np1/p2P4/1nP5/2N2NP1/PP2PPBP/R1BQ1RK1 w - - 0 11",
        "r2qkbnr/pp1n1pp1/2p1p2p/7P/3P4/3Q1NN1/PPP2PP1/R1B1K2R w KQkq - 0 11",
        "rnbqkb1r/1p3ppp/p4n2/3p4/3p4/2N1PN2/PPQ1BPPP/R1B1K2R w KQkq - 0 9",
        "rnbq1rk1/pp2ppbp/1n4p1/2p5/3P4/1BN1PN2/PP3PPP/R1BQK2R w KQ - 0 9",
        "rnbqkb1r/1p3ppp/p4n2/2pp4/3P4/2N1PN2/PPQ2PPP/R1B1KB1R w KQkq - 0 8",
        "rnbqkbnr/pp2pppp/8/2pp4/8/1P2PN2/P1PP1PPP/RNBQKB1R b KQkq - 0 3",
        "3r1rk1/pp3pp1/1qpbp2p/3n3P/2NP3Q/8/PPPB1PP1/2KR3R b - - 8 19",
        "1nb2rk1/rp3ppp/p7/3Nb3/3N4/8/PP2BPPP/3RK2R w K - 2 17",
        "r2q1rk1/pb1nbpp1/1p2pn1p/2pp4/2PP4/2NBPNB1/PP3PPP/R2Q1RK1 w - - 0 11",
        "2r2b1k/r4p1p/p5p1/1p1N3P/3R1P2/6K1/PP4P1/7R w - - 1 25",
        "r3r1k1/5pp1/p1p2b2/7p/3R1p1P/2N2P2/PPP3P1/1K5R w - - 3 24",
        "3r2qk/pp1r1pp1/1np1p2p/7P/1BPP4/P5R1/1P2QPP1/1K2R3 b - - 0 29",
        "8/P5R1/4kp2/7p/r4r1P/8/2P5/2K4R b - - 0 38",
        "2R2bk1/7p/p5p1/1p2N3/5P2/6K1/PP1r2P1/8 b - - 5 32",
        "5bk1/7p/6p1/R3N3/5P2/P5K1/r5P1/8 b - - 0 38",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
        "7r/pb3p2/2pR4/2P1nP2/PP2PkB1/2N4P/6K1/8 b - - 2 47",
        "6k1/8/pB2p1p1/3p3p/7q/PB6/Q7/2R3K1 w - - 0 37",
        "8/p4p2/7p/1P6/3b1P2/2kB2P1/6K1/8 w - - 5 35",
        "1r4k1/Q4ppp/8/8/4P3/8/K4PPP/1r3BR1 w - - 1 36"
    };

    u64 nodes = 0;
    u64 time = 0;

    for (auto& fen : FENS) {
        stringstream ss(fen);

        Board board;
        board.from_fen(ss);

        Thread engine {};

        STOP = FALSE;
        TIME_START = now();
        TIME_SOFT = UINT64_MAX;
        TIME_LIMIT = UINT64_MAX;
        VISITED_COUNT = 0;

        u64 time_1 = now();

        engine.start(board, 0, 15, TRUE);

        u64 time_2 = now();

        nodes += engine.nodes;
        time += time_2 - time_1;
    }

    cout << nodes << " nodes " << (nodes * 1000 / max(time, u64(1))) << " nps" << endl;
}

#endif

#ifdef OB_MINI
i32 main(i32 argc, char *argv[]) {
#else
i32 main() {
#endif
    // Zobrist hash init
    mt19937_64 rng;

    for (i32 i = 0; i < 832; i++)
        KEYS[i / 64][i % 64] = rng();

    // Search data
    Board board;

#ifdef OB_MINI
    // Bench
    if (argc > 1 && std::string(argv[1]) == "bench") {
        bench();
        return 0;
    }
#endif

#ifdef OB
    // Perft
    if (argc > 2 && std::string(argv[1]) == "perft") {
        board.startpos();

        u64 time_1 = now();
        u64 nodes = perft(board, std::stoi(argv[2]), true);
        u64 time_2 = now();

        cout << nodes << " nodes " << (nodes * 1000 / max(time_2 - time_1, u64(1))) << " nps" << endl;

        return 0;
    }
#endif

    // Uci
    string token;
    cin >> token;

#ifdef OB
    cout << "id name c4ke v2.0" << endl;
    cout << "id author citrus610 and cj5716" << endl;
    cout << "option name Hash type spin default 8 min 1 max 67108864" << endl;
    cout << "option name Threads type spin default 1 min 1 max 2048" << endl;
#endif

    cout << "uciok\n";

    for (; getline(cin, token);) {
        stringstream tokens(token);
        tokens >> token;

        // Uci isready
        if (token[0] == 'i')
            cout << "readyok\n";
#ifdef OB
        // Uci ucinewgame
        else if (token[0] == 'u') {
            memset(TTABLE, 0, sizeof(TTEntry) * (1ull << TT_BITS));
        }
        // Uci setoption
        else if (token[0] == 's') {
            tokens >> token >> token;

            if (token == "Hash") {
                u64 tt_size;

                tokens >> token >> tt_size;

                tt_size *= 1024 * 1024 / sizeof(TTEntry);

                TT_BITS = 63 - __builtin_clzll(tt_size);
                TT_SHIFT = 64 - TT_BITS;

                free(TTABLE);

                TTABLE = (TTEntry*)calloc(1ull << TT_BITS, sizeof(TTEntry));
            }

            if (token == "Threads") {
                tokens >> token >> THREADS;
            }
        }
#endif
        // Uci position
        else if (token[0] == 'p') {
            board.startpos();
            VISITED_COUNT = 0;

#ifdef OB
            tokens >> token;

            if (token[0] == 'f') {
                board.from_fen(tokens);
            }

            tokens >> token;
#else
            tokens >> token >> token;
#endif

            for (; tokens >> token;)
                BEST_MOVE = move_make(token[0] + token[1] * 8 - 489, token[2] + token[3] * 8 - 489, token[4] % 35 * 5 % 6),
                VISITED[VISITED_COUNT++] = board.hash,
                VISITED_COUNT *= board.board[move_to(BEST_MOVE)] > BLACK_KING && board.board[move_from(BEST_MOVE)] > BLACK_PAWN,
                board.make(BEST_MOVE);
        }
        // Uci go
        else if (token[0] == 'g') {
            u64 time;

#ifdef OB
            time = 1ull << 32;

            while (tokens >> token) {
                if (token == "wtime" && board.stm == WHITE) {
                    tokens >> time;
                }

                if (token == "btime" && board.stm == BLACK) {
                    tokens >> time;
                }
            }
#else
            tokens >> token >> time;

            if (board.stm)
                tokens >> token >> time;
#endif

            STOP = FALSE;
            TIME_START = now();
            TIME_SOFT = time / 20;
            TIME_LIMIT = TIME_START + time / 2;

#ifdef OB
            vector<jthread> threads(THREADS);
#else
            jthread threads[THREADS];
#endif

            for (i32 id = 0; id < THREADS; id++)
                threads[id] = jthread([=] { Thread{}.start(board, id); });

#ifdef OB
            bool is_quitting = false;

            if (time == 1ull << 32) {
                while (getline(cin, token)) {
                    if (token == "stop") {
                        break;
                    }

                    if (token == "quit") {
                        is_quitting = true;
                        break;
                    }

                    if (token == "isready") {
                        cout << "readyok" << endl;
                    }
                }

                STOP++;
            }

            if (is_quitting) {
                break;
            }
#endif
        }
        // Uci quit
        else if (token[0] == 'q')
            break;
    }

    free(TTABLE);
};