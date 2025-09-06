#include "search.cpp"

#ifdef OB
string move_str(u16 move) {
    string str;

    str += 97 + move_from(move) % 8;
    str += 49 + move_from(move) / 8;
    str += 97 + move_to(move) % 8;
    str += 49 + move_to(move) / 8;

    if (move_promo(move)) {
        str += " nbrq"[move_promo(move)];
    }

    return str;
}

u64 perft(Board& board, int depth, bool is_root = FALSE) {
    if (depth <= 0) {
        return 1;
    }

    u64 nodes = 0;

    u16 moves[MAX_MOVE];
    int count = board.movegen(moves, TRUE);

    for (int i = 0; i < count; i++) {
        Board child = board;

        if (!child.make(moves[i])) {
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

void test_perft() {
    struct Test {
        string fen;
        int depth;
        u64 result;
    };

    std::vector<Test> tests = {
        Test { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 6, 119060324 },
        Test { "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 5, 193690690 },
        Test { "4k3/8/8/8/8/8/8/4K2R w K - 0 1", 6, 764643 },
        Test { "4k3/8/8/8/8/8/8/R3K3 w Q - 0 1", 6, 846648 },
        Test { "4k2r/8/8/8/8/8/8/4K3 w k - 0 1", 6, 899442 },
        Test { "r3k3/8/8/8/8/8/8/4K3 w q - 0 1", 6, 1001523 },
        Test { "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1", 6, 2788982 },
        Test { "r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1", 6, 3517770 },
        Test { "8/8/8/8/8/8/6k1/4K2R w K - 0 1", 6, 185867 },
        Test { "8/8/8/8/8/8/1k6/R3K3 w Q - 0 1", 6, 413018 },
        Test { "4k2r/6K1/8/8/8/8/8/8 w k - 0 1", 6, 179869 },
        Test { "r3k3/1K6/8/8/8/8/8/8 w q - 0 1", 6, 367724 },
        Test { "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", 6, 179862938 },
        Test { "r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1", 6, 195629489 },
        Test { "r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1", 6, 184411439 },
        Test { "r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1", 6, 189224276 },
        Test { "1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 6, 198328929 },
        Test { "2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 6, 185959088 },
        Test { "r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1", 6, 190755813 },
        Test { "4k3/8/8/8/8/8/8/4K2R b K - 0 1", 6, 899442 },
        Test { "4k3/8/8/8/8/8/8/R3K3 b Q - 0 1", 6, 1001523 },
        Test { "4k2r/8/8/8/8/8/8/4K3 b k - 0 1", 6, 764643 },
        Test { "r3k3/8/8/8/8/8/8/4K3 b q - 0 1", 6, 846648 },
        Test { "4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1", 6, 3517770 },
        Test { "r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1", 6, 2788982 },
        Test { "8/8/8/8/8/8/6k1/4K2R b K - 0 1", 6, 179869 },
        Test { "8/8/8/8/8/8/1k6/R3K3 b Q - 0 1", 6, 367724 },
        Test { "4k2r/6K1/8/8/8/8/8/8 b k - 0 1", 6, 185867 },
        Test { "r3k3/1K6/8/8/8/8/8/8 b q - 0 1", 6, 413018 },
        Test { "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1", 6, 179862938 },
        Test { "r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1", 6, 198328929 },
        Test { "r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1", 6, 185959088 },
        Test { "r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1", 6, 190755813 },
        Test { "1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 6, 195629489 },
        Test { "2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 6, 184411439 },
        Test { "r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1", 6, 189224276 },
        Test { "8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1", 6, 8107539 },
        Test { "8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1", 6, 2594412 },
        Test { "8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1", 6, 19870403 },
        Test { "K7/8/2n5/1n6/8/8/8/k6N w - - 0 1", 6, 588695 },
        Test { "k7/8/2N5/1N6/8/8/8/K6n w - - 0 1", 6, 688780 },
        Test { "8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1", 6, 8503277 },
        Test { "8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1", 6, 3147566 },
        Test { "8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1", 6, 4405103 },
        Test { "K7/8/2n5/1n6/8/8/8/k6N b - - 0 1", 6, 688780 },
        Test { "k7/8/2N5/1N6/8/8/8/K6n b - - 0 1", 6, 588695 },
        Test { "B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1", 6, 22823890 },
        Test { "8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1", 6, 28861171 },
        Test { "k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1", 6, 7881673 },
        Test { "K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1", 6, 7382896 },
        Test { "B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1", 6, 9250746 },
        Test { "8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1", 6, 29027891 },
        Test { "k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1", 6, 7382896 },
        Test { "K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1", 6, 7881673 },
        Test { "7k/RR6/8/8/8/8/rr6/7K w - - 0 1", 6, 44956585 },
        Test { "R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1", 6, 525169084 },
        Test { "7k/RR6/8/8/8/8/rr6/7K b - - 0 1", 6, 44956585 },
        Test { "R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1", 6, 524966748 },
        Test { "6kq/8/8/8/8/8/8/7K w - - 0 1", 6, 391507 },
        Test { "6KQ/8/8/8/8/8/8/7k b - - 0 1", 6, 391507 },
        Test { "K7/8/8/3Q4/4q3/8/8/7k w - - 0 1", 6, 3370175 },
        Test { "6qk/8/8/8/8/8/8/7K b - - 0 1", 6, 419369 },
        Test { "6KQ/8/8/8/8/8/8/7k b - - 0 1", 6, 391507 },
        Test { "K7/8/8/3Q4/4q3/8/8/7k b - - 0 1", 6, 3370175 },
        Test { "8/8/8/8/8/K7/P7/k7 w - - 0 1", 6, 6249 },
        Test { "8/8/8/8/8/7K/7P/7k w - - 0 1", 6, 6249 },
        Test { "K7/p7/k7/8/8/8/8/8 w - - 0 1", 6, 2343 },
        Test { "7K/7p/7k/8/8/8/8/8 w - - 0 1", 6, 2343 },
        Test { "8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1", 6, 34834 },
        Test { "8/8/8/8/8/K7/P7/k7 b - - 0 1", 6, 2343 },
        Test { "8/8/8/8/8/7K/7P/7k b - - 0 1", 6, 2343 },
        Test { "K7/p7/k7/8/8/8/8/8 b - - 0 1", 6, 6249 },
        Test { "7K/7p/7k/8/8/8/8/8 b - - 0 1", 6, 6249 },
        Test { "8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1", 6, 34822 },
        Test { "8/8/8/8/8/4k3/4P3/4K3 w - - 0 1", 6, 11848 },
        Test { "4k3/4p3/4K3/8/8/8/8/8 b - - 0 1", 6, 11848 },
        Test { "8/8/7k/7p/7P/7K/8/8 w - - 0 1", 6, 10724 },
        Test { "8/8/k7/p7/P7/K7/8/8 w - - 0 1", 6, 10724 },
        Test { "8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1", 6, 53138 },
        Test { "8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1", 6, 157093 },
        Test { "8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1", 6, 158065 },
        Test { "k7/8/3p4/8/3P4/8/8/7K w - - 0 1", 6, 20960 },
        Test { "8/8/7k/7p/7P/7K/8/8 b - - 0 1", 6, 10724 },
        Test { "8/8/k7/p7/P7/K7/8/8 b - - 0 1", 6, 10724 },
        Test { "8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1", 6, 53138 },
        Test { "8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1", 6, 158065 },
        Test { "8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1", 6, 157093 },
        Test { "k7/8/3p4/8/3P4/8/8/7K b - - 0 1", 6, 21104 },
        Test { "7k/3p4/8/8/3P4/8/8/K7 w - - 0 1", 6, 32191 },
        Test { "7k/8/8/3p4/8/8/3P4/K7 w - - 0 1", 6, 30980 },
        Test { "k7/8/8/7p/6P1/8/8/K7 w - - 0 1", 6, 41874 },
        Test { "k7/8/7p/8/8/6P1/8/K7 w - - 0 1", 6, 29679 },
        Test { "k7/8/8/6p1/7P/8/8/K7 w - - 0 1", 6, 41874 },
        Test { "k7/8/6p1/8/8/7P/8/K7 w - - 0 1", 6, 29679 },
        Test { "k7/8/8/3p4/4p3/8/8/7K w - - 0 1", 6, 22886 },
        Test { "k7/8/3p4/8/8/4P3/8/7K w - - 0 1", 6, 28662 },
        Test { "7k/3p4/8/8/3P4/8/8/K7 b - - 0 1", 6, 32167 },
        Test { "7k/8/8/3p4/8/8/3P4/K7 b - - 0 1", 6, 30749 },
        Test { "k7/8/8/7p/6P1/8/8/K7 b - - 0 1", 6, 41874 },
        Test { "k7/8/7p/8/8/6P1/8/K7 b - - 0 1", 6, 29679 },
        Test { "k7/8/8/6p1/7P/8/8/K7 b - - 0 1", 6, 41874 },
        Test { "k7/8/6p1/8/8/7P/8/K7 b - - 0 1", 6, 29679 },
        Test { "k7/8/8/3p4/4p3/8/8/7K b - - 0 1", 6, 22579 },
        Test { "k7/8/3p4/8/8/4P3/8/7K b - - 0 1", 6, 28662 },
        Test { "7k/8/8/p7/1P6/8/8/7K w - - 0 1", 6, 41874 },
        Test { "7k/8/p7/8/8/1P6/8/7K w - - 0 1", 6, 29679 },
        Test { "7k/8/8/1p6/P7/8/8/7K w - - 0 1", 6, 41874 },
        Test { "7k/8/1p6/8/8/P7/8/7K w - - 0 1", 6, 29679 },
        Test { "k7/7p/8/8/8/8/6P1/K7 w - - 0 1", 6, 55338 },
        Test { "k7/6p1/8/8/8/8/7P/K7 w - - 0 1", 6, 55338 },
        Test { "3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1", 6, 199002 },
        Test { "7k/8/8/p7/1P6/8/8/7K b - - 0 1", 6, 41874 },
        Test { "7k/8/p7/8/8/1P6/8/7K b - - 0 1", 6, 29679 },
        Test { "7k/8/8/1p6/P7/8/8/7K b - - 0 1", 6, 41874 },
        Test { "7k/8/1p6/8/8/P7/8/7K b - - 0 1", 6, 29679 },
        Test { "k7/7p/8/8/8/8/6P1/K7 b - - 0 1", 6, 55338 },
        Test { "k7/6p1/8/8/8/8/7P/K7 b - - 0 1", 6, 55338 },
        Test { "3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1", 6, 199002 },
        Test { "8/Pk6/8/8/8/8/6Kp/8 w - - 0 1", 6, 1030499 },
        Test { "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1", 6, 37665329 },
        Test { "8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1", 6, 28859283 },
        Test { "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1", 6, 71179139 },
        Test { "8/Pk6/8/8/8/8/6Kp/8 b - - 0 1", 6, 1030499 },
        Test { "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1", 6, 37665329 },
        Test { "8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1", 6, 28859283 },
        Test { "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 6, 71179139 },
        Test { "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 6, 11030083 },
        Test { "rnbqkb1r/ppppp1pp/7n/4Pp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3", 5, 11139762 }
    };

    int passed = 0;

    for (auto& test : tests) {
        stringstream fen(test.fen);

        Board board;
        board.from_fen(fen);

        u64 result = perft(board, test.depth);

        if (result != test.result) {
            cout << "FAILED: \n";
            cout << "- fen: " << test.fen << "\n";
            cout << "- depth: " << test.depth << "\n";
            cout << "- expect: " << test.result << "\n";
            cout << "- result: " << result << "\n";

            break;
        }
        else {
            passed += 1;

            cout << "PASSED: " << passed << "/" << tests.size() << "\n";
        }
    }
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

        u64 visited[VISIT_SIZE];

        RUNNING = TRUE;
        LIMIT_SOFT = UINT64_MAX;
        LIMIT_HARD = UINT64_MAX;

        u64 time_1 = now();

        engine.start(board, visited, 0, 15, TRUE);

        u64 time_2 = now();

        nodes += engine.nodes;
        time += time_2 - time_1;
    }

    cout << nodes << " nodes " << (nodes * 1000 / max(time, u64(1))) << " nps" << endl;
}

#endif

#ifdef OB_MINI
int main(int argc, char *argv[]) {
#else
int main() {
#endif
    // Zobrist hash init
    mt19937_64 rng;

    for (int i = 0; i < 13; i++)
        for (int k = 0; k < 64; k++)
            KEYS[i][k] = rng();

    // Search data
    Board board;
    u64 visited[VISIT_SIZE];
    int visited_count = 0;
    TTABLE = (TTEntry*)calloc(1ull << TT_BITS, 8);

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
        if (std::string(argv[2]) == "test") {
            test_perft();
        }
        else {
            Board board;

            u64 time_1 = now();
            u64 nodes = perft(board, std::stoi(argv[2]), true);
            u64 time_2 = now();

            cout << nodes << " nodes " << (nodes * 1000 / max(time_2 - time_1, u64(1))) << " nps" << endl;
        }

        return 0;
    }
#endif

    // Uci
    string token;

    cin >> token;
    cout << "uciok\n";

    while (getline(cin, token)) {
        stringstream tokens(token);
        tokens >> token;

        // Uci isready
        if (token[0] == 'i') {
            cout << "readyok\n";
        }
#ifdef OB
        // Uci ucinewgame
        else if (token[0] == 'u') {
            memset(TTABLE, 0, sizeof(TTEntry) * (1ull << TT_BITS));
        }
#endif
        // Uci position
        else if (token[0] == 'p') {
            board = Board();
            visited_count = 0;

#ifdef OB
            tokens >> token;

            if (token[0] == 'f') {
                board.from_fen(tokens);
            }

            tokens >> token;
#else
            tokens >> token >> token;
#endif

            while (tokens >> token) {
                visited[visited_count++] = board.hash;
                board.make(move_make(token[0] + token[1] * 8 - 489, token[2] + token[3] * 8 - 489, token[4] % 35 * 5 % 6));
            }
        }
        // Uci go
        else if (token[0] == 'g') {
            u64 time;

            tokens >> token >> time;

            if (board.stm)
                tokens >> token >> time;

            RUNNING = TRUE;
            LIMIT_SOFT = now() + time / 50;
            LIMIT_HARD = now() + time / 2;

            thread t([&] () {
                Thread().start(board, visited, visited_count);
            });

            t.join();

            cout << "bestmove ";
            move_print(BEST_MOVE);
        }
        // Uci quit
        else if (token[0] == 'q') {
            break;
        }
    }

    free(TTABLE);

    return 0;
};