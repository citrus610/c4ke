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
            S(0, 0), S(-24, -16), S(-38, -28), S(-24, -28), S(-10, -18), S(43, 20), S(66, 116), S(0, 0),
            S(-25, -29), S(-7, -15), S(5, 3), S(22, 30), S(34, 38), S(62, 7), S(36, -11), S(-127, -22),
            S(-1, -17), S(15, -13), S(17, 4), S(12, 6), S(15, 9), S(23, 1), S(-8, -3), S(-73, 12),
            S(6, -24), S(-6, -28), S(-5, -23), S(-15, 0), S(6, 9), S(23, 10), S(2, 30), S(-11, 26),
            S(11, -47), S(18, -44), S(6, -5), S(1, 13), S(-4, 31), S(16, 17), S(-13, 25), S(-34, 11),
            S(-15, -41), S(47, -22), S(-32, -1), S(-98, 24), S(-92, 46), S(2, 52), S(14, 41), S(19, -54)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "PST_FILE",
        .data = {
            S(-23, 18), S(-10, 15), S(-6, -3), S(6, -15), S(17, -11), S(14, -2), S(16, 4), S(-14, -6),
            S(-37, -19), S(-15, -3), S(-5, 14), S(15, 18), S(18, 17), S(17, 4), S(6, -4), S(0, -27),
            S(-16, -6), S(2, 0), S(3, -1), S(-3, 11), S(5, 8), S(-2, 7), S(11, -2), S(0, -17),
            S(-14, 6), S(-15, 12), S(-7, 14), S(3, 1), S(15, -10), S(4, -3), S(18, -10), S(-4, -10),
            S(-15, -18), S(-12, -7), S(-11, 8), S(-2, 11), S(0, 17), S(5, 5), S(18, -7), S(16, -10),
            S(9, -63), S(37, -14), S(-3, 15), S(-60, 35), S(-26, 24), S(-47, 26), S(28, -6), S(19, -58)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "MOBILITY",
        .data = {
            S(9, 6), S(8, 7), S(3, 6), S(1, 13), S(-9, -4)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "PASSER",
        .data = {
            S(9, -26), S(11, 7), S(6, 69), S(24, 129), S(4, 209), S(38, 208)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PHALANX",
        .data = {
            S(8, 4), S(19, 21), S(33, 39), S(65, 100), S(174, 266), S(151, 257)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "THREAT",
        .data = {
            S(70, 27), S(78, 53), S(97, -5), S(85, -65)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "KING_ATTACK",
        .data = {
            S(12, -10), S(24, -5), S(34, -14), S(18, 20)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_PASSER_US",
        .data = {
            S(-7, 32), S(3, 63), S(-3, 26), S(-18, 7), S(-3, -23), S(4, -37), S(26, -45), S(-9, -34)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "KING_PASSER_THEM",
        .data = {
            S(-94, -53), S(-10, -69), S(4, -35), S(-14, -2), S(-18, 24), S(-23, 41), S(-29, 54), S(-26, 33)
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
    result += "    return data[index] + data[index + INDEX_EG] * 0x10000 - S(32, 32);\n";
    result += "}";

    return result;
};