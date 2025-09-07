#include <bits/stdc++.h>

constexpr int S(int mg, int eg)
{
    return mg + (eg << 16);
};

constexpr int get_mg(int score)
{
    return int16_t(score);
};

constexpr int get_eg(int score)
{
    return (score + 0x8000) >> 16;
};

struct Param
{
    std::string name;
    std::vector<int> data;
};

struct Compressed
{
    std::string str_mg;
    std::string str_eg;
    int min_mg;
    int min_eg;
};

inline const std::vector<Param> PARAMS = {
    Param {
        .name = "PST_RANK",
        .data = {
            0, S(-3, 0), S(-3, -1), S(-1, -1), S(1, 0), S(5, 3), 0, 0,
            S(-2, -5), S(0, -3), S(1, -1), S(3, 3), S(4, 4), S(5, 1), S(2, 0), S(-15, 1),
            S(0, -2), S(2, -1), S(2, 0), S(2, 0), S(2, 0), S(2, 0), S(-1, 0), S(-10, 2),
            S(0, -3), S(-1, -3), S(-2, -2), S(-2, 0), S(0, 2), S(2, 2), S(1, 3), S(2, 1),
            S(2, -11), S(3, -8), S(2, -3), S(0, 2), S(0, 5), S(-1, 5), S(-4, 7), S(-2, 4),
            S(-1, -6), S(1, -2), S(-1, 0), S(-4, 3), S(-1, 5), S(5, 4), S(5, 2), S(5, -6),
        }
    },
    Param {
        .name = "PST_FILE",
        .data = {
            S(-1, 1), S(-2, 1), S(-1, 0), S(0, -1), S(1, 0), S(2, 0), S(2, 0), S(-1, 0),
            S(-4, -3), S(-1, -1), S(0, 1), S(2, 3), S(2, 3), S(2, 0), S(1, -1), S(-1, -3),
            S(-2, -1), 0, S(1, 0), S(0, 1), S(1, 1), S(0, 1), S(2, 0), S(-1, -1),
            S(-2, 0), S(-1, 1), S(0, 1), S(1, 0), S(2, -1), S(1, 0), S(1, 0), S(-1, -1),
            S(-2, -3), S(-1, -1), S(-1, 0), S(0, 1), S(0, 2), S(1, 2), S(2, 0), S(1, -1),
            S(-2, -5), S(2, -1), S(-1, 1), S(-4, 2), S(-4, 2), S(-2, 2), S(2, -1), S(0, -5),
        }
    },
    Param {
        .name = "MOBILITY",
        .data = {
            0, S(8, 5), S(7, 7), S(3, 5), S(3, 2), S(-5, -1)
        }
    },
    Param {
        .name = "PASSER",
        .data = {
            0, S(0, 2), S(-1, 4), S(-2, 6), S(2, 10), S(3, 20), S(5, 30)
        }
    },
};

inline Compressed get_compressed_data(std::vector<int> data)
{
    auto result = Compressed();

    result.min_mg = INT32_MAX;
    result.min_eg = INT32_MAX;

    for (auto& score : data) {
        result.min_mg = std::min(result.min_mg, get_mg(score));
        result.min_eg = std::min(result.min_eg, get_eg(score));
    }

    for (auto& score : data) {
        int mg = get_mg(score) - result.min_mg + 32;
        int eg = get_eg(score) - result.min_eg + 32;

        result.str_mg.push_back(char(mg));
        result.str_eg.push_back(char(eg));
    }

    return result;
};

inline std::string get_eval_str()
{
    std::string result;
    std::string mg;
    std::string eg;
    std::string index;
    std::string offset;

    int index_eg = 0;

    for (auto& param : PARAMS) {
        auto compressed = get_compressed_data(param.data);

        mg += compressed.str_mg;
        eg += compressed.str_eg;
        index += "#define INDEX_" + param.name + " " + std::to_string(index_eg) + "\n";
        offset += "#define OFFSET_" + param.name + " S(" + std::to_string(compressed.min_mg) + ", " + std::to_string(compressed.min_eg) + ")\n";

        index_eg += param.data.size();
    }

    result += "#define DATA_STR \"";

    for (auto c : mg) {
        if (c == '\\' || c == '\"') {
            result += "\\";
        }

        result += c;
    }

    for (auto c : eg) {
        if (c == '\\' || c == '\"') {
            result += "\\";
        }

        result += c;
    }

    result += "\"\n\n";

    result += std::string("#define INDEX_EG ") + std::to_string(index_eg) + "\n\n";
    result += index + "\n";
    result += offset + "\n";

    result += "int get_data(int index) {\n";
    result += "    auto data = DATA_STR;\n\n";
    result += "    return data[index] + data[index + INDEX_EG] * 0x10000 - S(32, 32);\n";
    result += "}";

    return result;
};