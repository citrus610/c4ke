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
            S(0, 0), S(-22, -25), S(-35, -36), S(-27, -32), S(-11, -23), S(45, 13), S(52, 120), S(0, 0),
            S(-30, -28), S(-10, -11), S(5, 7), S(26, 32), S(46, 36), S(60, 6), S(35, -14), S(-131, -28),
            S(-6, -15), S(11, -11), S(17, 4), S(16, 6), S(27, 6), S(21, 2), S(-11, -3), S(-75, 11),
            S(3, -25), S(-8, -27), S(-6, -22), S(-15, 2), S(9, 11), S(17, 10), S(-2, 31), S(1, 20),
            S(9, -52), S(16, -48), S(5, -8), S(1, 15), S(1, 34), S(9, 30), S(-18, 34), S(-24, -4),
            S(-20, -32), S(42, -12), S(-37, 6), S(-102, 21), S(-81, 26), S(21, 27), S(35, 23), S(21, -49)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "PST_FILE",
        .data = {
            S(-19, 14), S(-9, 14), S(-5, -4), S(5, -16), S(15, -9), S(13, -1), S(14, 6), S(-14, -5),
            S(-41, -20), S(-14, -3), S(-3, 15), S(15, 19), S(17, 18), S(19, 4), S(8, -5), S(-2, -29),
            S(-18, -6), S(2, 0), S(4, -1), S(-3, 11), S(4, 9), S(-1, 7), S(13, -1), S(-1, -18),
            S(-14, 5), S(-15, 11), S(-6, 13), S(4, 1), S(16, -10), S(4, -1), S(17, -8), S(-6, -11),
            S(-15, -19), S(-12, -5), S(-12, 12), S(-4, 15), S(-2, 21), S(6, 5), S(20, -11), S(18, -18),
            S(5, -56), S(38, -18), S(-1, 13), S(-58, 34), S(-24, 22), S(-44, 22), S(29, -8), S(16, -53)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "MOBILITY",
        .data = {
            S(9, 5), S(8, 7), S(3, 6), S(2, 10), S(-9, -2)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "PASSER",
        .data = {
            S(15, -27), S(10, 8), S(2, 72), S(12, 138), S(-36, 235), S(20, 228)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PHALANX",
        .data = {
            S(9, 4), S(17, 21), S(32, 37), S(59, 102), S(159, 293), S(154, 294)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "THREAT",
        .data = {
            S(76, 27), S(81, 56), S(104, -6), S(89, -58)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PUSH_THREAT",
        .data = {
            S(29, 9), S(29, -7), S(20, 19), S(25, -16)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_ATTACK",
        .data = {
            S(8, 20), S(16, 31), S(45, -27), S(18, 19)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_PASSER_US",
        .data = {
            S(-74, 68), S(-49, 94), S(-14, 37), S(-23, 13), S(-3, -18), S(7, -35), S(40, -49), S(8, -37)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "KING_PASSER_THEM",
        .data = {
            S(-97, -4), S(40, -80), S(1, -41), S(-16, -6), S(-21, 24), S(-28, 44), S(-52, 68), S(-64, 60)
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

    int offset_king_passer_mg = 0;
    int offset_king_passer_eg = 0;

    for (auto& param : PARAMS) {
        auto compressed = get_compressed_data(param.data, param.scale);

        mg += compressed.str_mg;
        eg += compressed.str_eg;
        index += "#define INDEX_" + param.name + " " + std::to_string(index_eg + param.index_offset) + "\n";

        if (param.name == "PST_FILE" || param.name == "PST_RANK") {
            offset_pst_mg += compressed.min_mg;
            offset_pst_eg += compressed.min_eg;
        }
        else if (param.name == "KING_PASSER_US" || param.name == "KING_PASSER_THEM") {
            offset_king_passer_mg += compressed.min_mg;
            offset_king_passer_eg += compressed.min_eg;
        }
        else {
            offset += "#define OFFSET_" + param.name + " S(" + std::to_string(compressed.min_mg) + ", " + std::to_string(compressed.min_eg) + ")\n";
        }

        index_eg += param.data.size();
    }

    offset += "#define OFFSET_PST S(" + std::to_string(offset_pst_mg) + ", " + std::to_string(offset_pst_eg) + ")\n";
    offset += "#define OFFSET_KING_PASSER S(" + std::to_string(offset_king_passer_mg) + ", " + std::to_string(offset_king_passer_eg) + ")\n";

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