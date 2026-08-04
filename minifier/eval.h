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
            S(0, 0), S(-23, -25), S(-36, -37), S(-26, -34), S(-10, -24), S(45, 15), S(48, 117), S(0, 0), 
            S(-33, -31), S(-11, -14), S(5, 10), S(25, 35), S(45, 38), S(60, 7), S(37, -14), S(-129, -31), 
            S(-8, -16), S(9, -17), S(16, 5), S(16, 5), S(27, 4), S(20, 1), S(-8, 1), S(-72, 17), 
            S(4, -30), S(-8, -32), S(-6, -25), S(-15, 3), S(8, 12), S(17, 12), S(-2, 35), S(2, 24), 
            S(11, -66), S(18, -62), S(6, -12), S(1, 20), S(-1, 44), S(8, 38), S(-19, 42), S(-25, -5), 
            S(-20, -52), S(42, -26), S(-35, -3), S(-98, 23), S(-77, 46), S(14, 55), S(32, 41), S(21, -59)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "PST_FILE",
        .data = {
            S(-19, 15), S(-9, 15), S(-4, -5), S(6, -18), S(15, -10), S(12, 0), S(13, 8), S(-13, -4), 
            S(-41, -21), S(-14, -5), S(-3, 19), S(14, 25), S(16, 21), S(20, 5), S(9, -10), S(-2, -34), 
            S(-18, -5), S(2, 0), S(5, 0), S(-3, 16), S(5, 12), S(-1, 6), S(12, -7), S(-1, -22), 
            S(-14, 6), S(-15, 13), S(-7, 15), S(4, 1), S(16, -11), S(4, -2), S(17, -10), S(-6, -13), 
            S(-14, -26), S(-12, -5), S(-12, 18), S(-4, 21), S(-2, 28), S(6, 6), S(20, -15), S(19, -28), 
            S(8, -70), S(37, -16), S(-1, 17), S(-60, 41), S(-25, 28), S(-45, 29), S(28, -7), S(18, -64)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "MOBILITY",
        .data = {
            S(9, 5), S(8, 9), S(3, 6), S(2, 9), S(-8, -4)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "PASSER",
        .data = {
            S(16, -59), S(11, -20), S(1, 53), S(10, 127), S(-38, 236), S(14, 243)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PHALANX",
        .data = {
            S(10, 4), S(15, 23), S(29, 43), S(55, 124), S(131, 388), S(168, 414)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "THREAT",
        .data = {
            S(75, 33), S(80, 64), S(103, -1), S(86, -49)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PUSH_THREAT",
        .data = {
            S(28, 7), S(27, -10), S(19, 24), S(22, -9)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_ATTACK",
        .data = {
            S(7, 20), S(15, 36), S(43, -26), S(19, 20)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_PASSER_US",
        .data = {
            S(-88, 57), S(-48, 88), S(-16, 29), S(-24, 1), S(-4, -36), S(7, -56), S(41, -71), S(11, -61)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "KING_PASSER_THEM",
        .data = {
            S(-100, -93), S(38, -60), S(1, -7), S(-16, 36), S(-21, 70), S(-27, 94), S(-52, 123), S(-64, 115)
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
    result += "    return data[index] + (data[index + INDEX_EG] - 32 << 16) - 32;\n";
    result += "}";

    return result;
};