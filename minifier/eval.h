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
    int scale;
    int index_offset;
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
            0, S(-15, -1), S(-25, -18), S(-5, -22), S(10, -5), S(37, 25), S(33, 72), 0,
            S(-19, -55), S(-3, -33), S(8, 1), S(26, 26), S(27, 42), S(66, 11), S(27, -4), S(-128, 15),
            S(-4, -28), S(17, -23), S(14, 1), S(15, 5), S(16, 13), S(27, 8), S(-9, 7), S(-69, 19),
            S(-5, -37), S(-27, -28), S(-19, -16), S(-21, 10), S(-4, 26), S(20, 20), S(20, 22), S(37, 8),
            S(0, -72), S(15, -64), S(6, -15), S(-5, 28), S(5, 25), S(3, 41), S(-15, 51), S(-6, 12),
            S(-26, -96), S(-8, -30), S(-61, -4), S(-67, 15), S(-44, 44), S(43, 72), S(169, 52), S(154, -3)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "PST_FILE",
        .data = {
            S(-21, 4), S(3, 6), S(1, 1), S(10, -11), S(7, -5), S(13, 2), S(14, 5), S(-26, -2),
            S(-24, -39), S(-2, -11), S(2, 14), S(15, 23), S(15, 22), S(9, 14), S(3, 0), S(-11, -23),
            S(-5, -17), S(2, 4), S(1, 4), S(0, 11), S(3, 8), S(-4, 8), S(7, 3), S(1, -20),
            S(-23, 6), S(-10, 8), S(4, 5), S(16, -1), S(20, -6), S(7, -1), S(-3, -1), S(-11, -8),
            S(-10, -12), S(-7, -3), S(-1, -3), S(4, 11), S(10, 1), S(7, 6), S(5, -1), S(-5, 7),
            S(10, -61), S(14, 6), S(-21, 22), S(-45, 30), S(-28, 18), S(-31, 20), S(22, 1), S(22, -65)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "MOBILITY",
        .data = {
            S(9, -1), S(8, 2), S(5, 3), S(3, 10), S(-5, -12)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "PASSER",
        .data = {
            S(-12, 7), S(-24, 29), S(-17, 55), S(3, 69), S(6, 114), S(14, 167)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PHALANX",
        .data = {
            S(7, 21), S(15, 28), S(22, 38), S(42, 89), S(168, 196), S(174, 251)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "THREAT",
        .data = {
            S(65, 59), S(65, 69), S(61, 54), S(51, 9)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_ATTACK",
        .data = {
            S(13, -11), S(15, -4), S(14, -6), S(9, 20)
        },
        .scale = 1,
        .index_offset = -1
    },
};

inline Compressed get_compressed_data(std::vector<int> data, int scale)
{
    auto result = Compressed();

    result.min_mg = INT32_MAX;
    result.min_eg = INT32_MAX;

    if (scale < 2) {
        scale = 1;
    }

    if (scale > 1) {
        for (auto& score : data) {
            int mg = int(std::round(double(get_mg(score)) / double(scale)));
            int eg = int(std::round(double(get_eg(score)) / double(scale)));

            score = S(mg, eg);
        }
    }

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

    int offset_pst_mg = 0;
    int offset_pst_eg = 0;

    for (auto& param : PARAMS) {
        auto compressed = get_compressed_data(param.data, param.scale);

        mg += compressed.str_mg;
        eg += compressed.str_eg;
        index += "#define INDEX_" + param.name + " " + std::to_string(index_eg + param.index_offset) + "\n";

        if (param.name == "PST_FILE" || param.name == "PST_RANK") {
            offset_pst_mg += compressed.min_mg;
            offset_pst_eg += compressed.min_eg;
        }
        else {
            offset += "#define OFFSET_" + param.name + " S(" + std::to_string(compressed.min_mg) + ", " + std::to_string(compressed.min_eg) + ")\n";
        }

        index_eg += param.data.size();
    }

    offset += "#define OFFSET_PST S(" + std::to_string(offset_pst_mg) + ", " + std::to_string(offset_pst_eg) + ")\n";

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