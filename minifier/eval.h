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
            S(0, 0), S(-21, -18), S(-35, -28), S(-26, -25), S(-11, -16), S(43, 23), S(56, 124), S(0, 0),
            S(-31, -28), S(-11, -13), S(5, 6), S(25, 31), S(45, 36), S(60, 6), S(35, -13), S(-129, -26),
            S(-6, -14), S(11, -11), S(16, 5), S(16, 5), S(27, 4), S(20, 2), S(-11, -2), S(-73, 11),
            S(4, -25), S(-7, -28), S(-5, -23), S(-14, 2), S(9, 11), S(17, 10), S(-1, 30), S(-4, 22),
            S(9, -49), S(15, -45), S(5, -7), S(2, 12), S(3, 29), S(10, 28), S(-18, 33), S(-26, -1),
            S(-22, -33), S(40, -12), S(-36, 6), S(-99, 21), S(-79, 27), S(20, 27), S(34, 24), S(23, -50)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "PST_FILE",
        .data = {
            S(-20, 14), S(-9, 14), S(-5, -4), S(5, -15), S(15, -9), S(13, -1), S(14, 6), S(-14, -4),
            S(-41, -19), S(-14, -3), S(-3, 14), S(15, 19), S(16, 19), S(20, 4), S(8, -4), S(-2, -28),
            S(-18, -5), S(2, 0), S(4, -1), S(-3, 11), S(5, 9), S(-1, 7), S(13, -2), S(-1, -18),
            S(-14, 5), S(-15, 11), S(-7, 14), S(4, 1), S(15, -9), S(4, -2), S(18, -9), S(-5, -10),
            S(-16, -17), S(-12, -6), S(-11, 9), S(-3, 13), S(-2, 19), S(6, 4), S(20, -9), S(17, -13),
            S(7, -58), S(38, -17), S(-1, 13), S(-58, 34), S(-24, 23), S(-45, 24), S(28, -8), S(17, -55)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "MOBILITY",
        .data = {
            S(9, 5), S(8, 7), S(3, 6), S(1, 12), S(-9, -3)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "PASSER",
        .data = {
            S(12, -25), S(8, 9), S(1, 71), S(14, 135), S(-25, 224), S(24, 223)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PHALANX",
        .data = {
            S(10, 4), S(17, 20), S(32, 36), S(60, 98), S(163, 279), S(152, 275)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "THREAT",
        .data = {
            S(76, 27), S(81, 55), S(102, -3), S(89, -61)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PUSH_THREAT",
        .data = {
            S(29, 6), S(29, -6), S(19, 20), S(26, -22)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_ATTACK",
        .data = {
            S(11, -7), S(23, -4), S(34, -14), S(18, 19)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_PASSER_US",
        .data = {
            S(-57, 59), S(-44, 88), S(-13, 34), S(-22, 11), S(-3, -19), S(6, -36), S(35, -47), S(1, -34)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "KING_PASSER_THEM",
        .data = {
            S(-82, -15), S(18, -75), S(1, -39), S(-15, -4), S(-20, 24), S(-25, 43), S(-45, 65), S(-48, 49)
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