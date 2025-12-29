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
            S(0, 0), S(-22, -25), S(-36, -36), S(-27, -32), S(-12, -22), S(45, 18), S(49, 125), S(0, 0),
            S(-31, -28), S(-11, -11), S(5, 9), S(26, 38), S(46, 40), S(60, 4), S(35, -19), S(-131, -33),
            S(-6, -15), S(10, -11), S(16, 7), S(15, 7), S(27, 6), S(21, 1), S(-11, -5), S(-73, 10),
            S(2, -29), S(-9, -31), S(-7, -24), S(-15, 4), S(8, 13), S(16, 12), S(-2, 34), S(5, 21),
            S(9, -58), S(16, -53), S(5, -9), S(1, 17), S(1, 39), S(9, 34), S(-18, 38), S(-23, -9),
            S(-26, -35), S(37, -13), S(-40, 7), S(-99, 23), S(-70, 27), S(28, 28), S(42, 25), S(20, -50)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "PST_FILE",
        .data = {
            S(-19, 16), S(-9, 15), S(-5, -5), S(6, -18), S(15, -10), S(13, 0), S(14, 7), S(-14, -4),
            S(-41, -19), S(-14, -2), S(-3, 16), S(15, 21), S(17, 20), S(20, 3), S(9, -7), S(-2, -32),
            S(-18, -5), S(2, -1), S(4, -1), S(-3, 12), S(4, 10), S(-1, 7), S(13, -3), S(-1, -20),
            S(-14, 6), S(-15, 13), S(-6, 15), S(4, 1), S(16, -11), S(4, -1), S(17, -10), S(-6, -12),
            S(-16, -19), S(-12, -5), S(-12, 13), S(-4, 17), S(-2, 23), S(6, 4), S(20, -13), S(18, -21),
            S(5, -62), S(38, -19), S(-1, 15), S(-59, 39), S(-24, 26), S(-44, 26), S(29, -9), S(16, -59)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "MOBILITY",
        .data = {
            S(9, 6), S(8, 9), S(3, 6), S(1, 13), S(-9, -2)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "PASSER",
        .data = {
            S(16, -44), S(11, -5), S(2, 66), S(12, 139), S(-38, 248), S(16, 247)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PHALANX",
        .data = {
            S(9, 5), S(17, 24), S(32, 41), S(58, 117), S(159, 330), S(159, 327)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "THREAT",
        .data = {
            S(76, 33), S(81, 64), S(104, -2), S(87, -54)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PUSH_THREAT",
        .data = {
            S(29, 9), S(29, -6), S(20, 23), S(24, -12)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_ATTACK",
        .data = {
            S(8, 21), S(17, 32), S(46, -37), S(18, 21)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_PASSER_US",
        .data = {
            S(-90, 80), S(-51, 104), S(-13, 36), S(-22, 8), S(-2, -27), S(9, -47), S(42, -63), S(11, -51)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "KING_PASSER_THEM",
        .data = {
            S(-89, -61), S(34, -73), S(-2, -30), S(-17, 10), S(-22, 43), S(-28, 67), S(-53, 96), S(-65, 87)
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