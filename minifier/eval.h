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
            S(0, 0), S(-29, -23), S(-36, -37), S(-23, -33), S(-12, -15), S(31, 9), S(58, 125), S(0, 0),
            S(-27, -38), S(-12, -21), S(4, 2), S(33, 28), S(38, 40), S(61, 11), S(27, -4), S(-125, -18),
            S(-13, -24), S(13, -25), S(20, -5), S(22, 1), S(24, 11), S(22, 6), S(-26, 15), S(-60, 22),
            S(-8, -42), S(-33, -33), S(-15, -25), S(-16, 4), S(2, 19), S(19, 21), S(6, 45), S(46, 11),
            S(1, -89), S(9, -89), S(1, -24), S(-2, 22), S(1, 53), S(5, 56), S(-16, 58), S(1, 13),
            S(-9, -39), S(34, -16), S(-36, 14), S(-93, 26), S(-79, 35), S(-19, 39), S(52, 12), S(27, -45)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "PST_FILE",
        .data = {
            S(-18, 6), S(-4, 9), S(-1, -2), S(10, -11), S(7, -3), S(13, 3), S(11, 4), S(-17, -5),
            S(-35, -22), S(-11, 1), S(-2, 13), S(14, 22), S(16, 21), S(17, 3), S(8, -2), S(-8, -36),
            S(-12, -14), S(3, 2), S(3, 6), S(1, 10), S(3, 8), S(-4, 11), S(11, -2), S(-5, -21),
            S(-20, 11), S(-14, 12), S(-1, 10), S(15, -4), S(21, -12), S(18, -6), S(-3, -2), S(-15, -9),
            S(-17, -24), S(-7, -12), S(1, -5), S(-2, 22), S(3, 21), S(9, 6), S(11, -1), S(3, -7),
            S(23, -57), S(32, -12), S(-3, 14), S(-57, 38), S(-30, 20), S(-37, 24), S(31, -8), S(18, -56)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "MOBILITY",
        .data = {
            S(9, 5), S(8, 7), S(5, 6), S(1, 10), S(-6, -3)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "PASSER",
        .data = {
            S(15, -25), S(11, 8), S(2, 75), S(11, 132), S(-4, 222), S(20, 247)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PHALANX",
        .data = {
            S(7, 10), S(11, 30), S(16, 42), S(41, 96), S(141, 274), S(161, 302)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "THREAT",
        .data = {
            S(72, 71), S(75, 89), S(84, 36), S(71, -35)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PUSH_THREAT",
        .data = {
            S(22, 5), S(24, -6), S(6, 26), S(21, -14)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_ATTACK",
        .data = {
            S(15, 25), S(16, 28), S(27, -8), S(15, 29)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_PASSER_US",
        .data = {
            S(-73, 92), S(-22, 91), S(-9, 37), S(-16, 12), S(-5, -19), S(2, -33), S(29, -51), S(4, -34)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "KING_PASSER_THEM",
        .data = {
            S(-114, 24), S(33, -81), S(5, -40), S(-10, -6), S(-19, 22), S(-22, 40), S(-40, 69), S(-50, 62)
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