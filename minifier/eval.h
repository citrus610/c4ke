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
            S(0, 0), S(-23, -22), S(-36, -34), S(-27, -32), S(-11, -22), S(45, 17), S(50, 116), S(0, 0),
            S(-31, -28), S(-11, -11), S(5, 9), S(26, 38), S(46, 39), S(60, 4), S(35, -18), S(-131, -32),
            S(-6, -15), S(10, -12), S(16, 7), S(15, 7), S(27, 5), S(21, 1), S(-11, -5), S(-74, 12),
            S(3, -30), S(-9, -31), S(-6, -24), S(-15, 4), S(8, 13), S(16, 12), S(-2, 34), S(5, 22),
            S(9, -59), S(16, -54), S(5, -9), S(1, 18), S(1, 40), S(9, 35), S(-18, 39), S(-23, -8),
            S(-23, -51), S(40, -26), S(-37, -3), S(-97, 23), S(-75, 45), S(18, 53), S(35, 40), S(21, -57)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "PST_FILE",
        .data = {
            S(-20, 16), S(-9, 15), S(-5, -5), S(6, -19), S(15, -11), S(13, -1), S(14, 8), S(-14, -4),
            S(-41, -19), S(-14, -2), S(-3, 16), S(15, 21), S(17, 20), S(20, 2), S(9, -7), S(-2, -32),
            S(-18, -4), S(2, 0), S(4, -2), S(-3, 12), S(4, 9), S(-1, 6), S(13, -3), S(-1, -19),
            S(-14, 6), S(-15, 13), S(-6, 15), S(4, 1), S(16, -11), S(4, -2), S(17, -10), S(-6, -12),
            S(-16, -20), S(-12, -5), S(-12, 14), S(-4, 17), S(-2, 24), S(6, 4), S(20, -13), S(18, -21),
            S(7, -68), S(37, -16), S(-1, 17), S(-59, 40), S(-24, 28), S(-45, 28), S(28, -7), S(17, -63)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "MOBILITY",
        .data = {
            S(9, 6), S(8, 9), S(3, 6), S(1, 13), S(-8, -4)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "PASSER",
        .data = {
            S(14, -57), S(9, -18), S(0, 53), S(11, 125), S(-37, 232), S(17, 236)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PHALANX",
        .data = {
            S(10, 4), S(17, 25), S(31, 44), S(57, 123), S(160, 341), S(160, 334)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "THREAT",
        .data = {
            S(76, 33), S(81, 65), S(103, -1), S(87, -53)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PUSH_THREAT",
        .data = {
            S(29, 7), S(29, -5), S(20, 24), S(24, -11)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_ATTACK",
        .data = {
            S(9, 18), S(17, 32), S(46, -36), S(18, 21)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_PASSER_US",
        .data = {
            S(-89, 57), S(-47, 86), S(-15, 27), S(-23, -1), S(-3, -36), S(7, -56), S(40, -71), S(9, -60)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "KING_PASSER_THEM",
        .data = {
            S(-104, -86), S(38, -60), S(0, -7), S(-15, 35), S(-20, 68), S(-26, 91), S(-51, 119), S(-62, 111)
        },
        .scale = 8,
        .index_offset = 0
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
    auto is_trigraph = [] (const std::string& str, size_t i) -> bool {
        bool is_double_question = str[i] == '?' && i + 1 < str.size() && str[i + 1] == '?';

        if (!is_double_question || i + 2 >= str.size()) {
            return false;
        }

        char list[] = { '=', '/', '(', ')', '!', '<', '>', '-' };

        for (auto c : list) {
            if (c == str[i + 2]) {
                return true;
            }
        }

        return false;
    };

    std::string result;
    std::string mg;
    std::string eg;
    std::string index;
    std::string offset;

    int index_eg = 0;

    int offset_pst_mg = 0;
    int offset_pst_eg = 0;

    int offset_passer_mg = 0;
    int offset_passer_eg = 0;

    for (auto& param : PARAMS) {
        auto compressed = get_compressed_data(param.data, param.scale);

        mg += compressed.str_mg;
        eg += compressed.str_eg;
        index += "#define INDEX_" + param.name + " " + std::to_string(index_eg + param.index_offset) + "\n";

        if (param.name == "PST_FILE" || param.name == "PST_RANK") {
            offset_pst_mg += compressed.min_mg;
            offset_pst_eg += compressed.min_eg;
        }
        else if (param.name == "PASSER" || param.name == "KING_PASSER_US" || param.name == "KING_PASSER_THEM") {
            offset_passer_mg += compressed.min_mg;
            offset_passer_eg += compressed.min_eg;
        }
        else {
            offset += "#define OFFSET_" + param.name + " S(" + std::to_string(compressed.min_mg) + ", " + std::to_string(compressed.min_eg) + ")\n";
        }

        index_eg += param.data.size();
    }

    offset += "#define OFFSET_PST S(" + std::to_string(offset_pst_mg) + ", " + std::to_string(offset_pst_eg) + ")\n";
    offset += "#define OFFSET_PASSER S(" + std::to_string(offset_passer_mg) + ", " + std::to_string(offset_passer_eg) + ")\n";

    result += "#define DATA_STR \"";

    for (size_t i = 0; i < mg.size(); ++i) {
        auto c = mg[i];

        if (c == '\\' || c == '\"') {
            result += "\\";
        }

        result += c;

        if (is_trigraph(mg, i)) {
            result += "\\";
        }
    }

    for (size_t i = 0; i < eg.size(); ++i) {
        auto c = eg[i];

        if (c == '\\' || c == '\"') {
            result += "\\";
        }

        result += c;

        if (is_trigraph(eg, i)) {
            result += "\\";
        }
    }

    result += "\"\n\n";

    result += std::string("#define INDEX_EG ") + std::to_string(index_eg) + "\n\n";
    result += index + "\n";
    result += offset + "\n";

    result += "i32 get_data(i32 index) {\n";
    result += "    auto data = DATA_STR;\n\n";
    result += "    return data[index] + (data[index + INDEX_EG] << 16) - S(32, 32);\n";
    result += "}";

    return result;
};