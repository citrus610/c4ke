#pragma once

#include "eval.h"
#include <bits/stdc++.h>

struct Entry
{
    std::vector<i32> coefs = {};
    f64 score = 0.0;
    f64 scale = 1.0;
    f64 phase = 0.0;
    f64 wdl = 0.0;
    bool is_white = true;
};

struct Dataset
{
    std::vector<Entry> data;
};

Entry get_data_entry(const std::string& str)
{
    // The data format is <FEN> | <score> | <wdl>
    // Example: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 | 20 | 0.0

    auto result = Entry();
    auto board = Board();

    board.set(str);

    auto trace = get_trace(board);

    result.coefs = get_coefs(trace);
    result.scale = trace.scale;
    result.phase = trace.phase;
    result.is_white = board.stm == color::WHITE;

    usize index_1 = str.find('|');
    usize index_2 = str.find('|', index_1 + 1);

    std::string str_score;
    std::string str_wdl;

    for (usize i = index_1; i < index_2; ++i) {
        auto c = str[i];

        if (c == ' ' || c == '|') {
            continue;
        }

        str_score.push_back(c);
    }

    result.score = std::stoi(str_score);

    for (usize i = index_2; i < str.size(); ++i) {
        auto c = str[i];

        if (c == ' ' || c == '|') {
            continue;
        }

        str_wdl.push_back(c);
    }

    if (str_wdl == "0.0") {
        result.wdl = 0.0;
    }
    else if (str_wdl == "1.0") {
        result.wdl = 1.0;
    }
    else {
        result.wdl = 0.5;
    }

    return result;
};

Dataset get_dataset(const std::string& path)
{
    Dataset result;

    auto file = std::ifstream(path);

    std::string line;

    usize positions = 0;
    usize positions_max = 1000000;

    std::cout << "Loading dataset\n";

    while (std::getline(file, line)) {
        result.data.push_back(get_data_entry(line));

        positions += 1;

        if ((positions % 1000) == 0) {
            std::cout << "\rLoaded " << positions << " data entries";
        }

        if (positions >= positions_max) {
            break;
        }
    }

    std::cout << "\nTotal " << positions << " data entries\n";

    for (usize i = 0; i < 5; ++i) {
        std::cout << result.data[i].phase << " " << result.data[i].wdl << " " << result.data[i].score << " " << result.data[i].is_white << "\n";
    }

    return result;
};