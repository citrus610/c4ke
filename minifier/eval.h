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
            S(0, 0), S(-22, -16), S(-36, -26), S(-22, -26), S(-8, -17), S(48, 17), S(48, 107), S(0, 0),
            S(-26, -28), S(-7, -15), S(5, 1), S(22, 28), S(34, 37), S(61, 7), S(36, -10), S(-125, -20),
            S(-2, -16), S(14, -13), S(18, 2), S(12, 5), S(15, 10), S(23, 2), S(-8, -2), S(-73, 13),
            S(5, -23), S(-6, -28), S(-6, -24), S(-15, 0), S(5, 10), S(22, 10), S(3, 29), S(-9, 25),
            S(11, -46), S(19, -42), S(7, -7), S(2, 9), S(-3, 28), S(16, 18), S(-14, 26), S(-38, 14),
            S(-16, -61), S(42, -22), S(-35, 6), S(-91, 36), S(-79, 61), S(8, 64), S(12, 45), S(15, -65)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "PST_FILE",
        .data = {
            S(-21, 20), S(-9, 16), S(-6, -3), S(6, -18), S(18, -14), S(13, -3), S(15, 6), S(-16, -4),
            S(-37, -18), S(-15, -3), S(-5, 13), S(15, 17), S(18, 17), S(17, 3), S(6, -4), S(0, -25),
            S(-16, -6), S(2, 0), S(3, -1), S(-3, 10), S(5, 8), S(-2, 7), S(11, -2), S(0, -17),
            S(-14, 6), S(-15, 12), S(-7, 14), S(4, 1), S(15, -9), S(4, -2), S(18, -10), S(-4, -11),
            S(-14, -17), S(-12, -8), S(-10, 4), S(-2, 8), S(0, 14), S(5, 6), S(17, -3), S(15, -3),
            S(9, -72), S(36, -13), S(-6, 20), S(-62, 43), S(-28, 32), S(-48, 29), S(28, -7), S(21, -68)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "MOBILITY",
        .data = {
            S(9, 7), S(8, 7), S(2, 6), S(0, 16), S(-9, -6)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "PASSER",
        .data = {
            S(-8, 3), S(-11, 23), S(-11, 62), S(13, 103), S(-1, 177), S(18, 196)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PHALANX",
        .data = {
            S(8, 4), S(19, 19), S(34, 35), S(67, 92), S(162, 237), S(147, 236)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "THREAT",
        .data = {
            S(70, 25), S(78, 51), S(97, -5), S(84, -62)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "KING_ATTACK",
        .data = {
            S(12, -11), S(24, -6), S(33, -14), S(18, 18)
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

    result += "int get_data(int index) {\n";
    result += "    auto data = DATA_STR;\n\n";
    result += "    return data[index] + data[index + INDEX_EG] * 0x10000 - S(32, 32);\n";
    result += "}";

    return result;
};