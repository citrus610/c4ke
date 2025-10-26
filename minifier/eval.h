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
            S(0, 0), S(-24, -15), S(-38, -26), S(-24, -26), S(-11, -16), S(44, 21), S(60, 118), S(0, 0),
            S(-25, -29), S(-7, -15), S(4, 4), S(21, 31), S(34, 39), S(62, 7), S(37, -11), S(-125, -25),
            S(-1, -17), S(14, -13), S(17, 4), S(11, 6), S(14, 10), S(23, 1), S(-8, -3), S(-71, 11),
            S(5, -24), S(-6, -28), S(-6, -23), S(-16, 0), S(4, 10), S(20, 11), S(1, 31), S(-1, 23),
            S(11, -53), S(18, -49), S(5, -8), S(-1, 16), S(-7, 39), S(13, 26), S(-15, 32), S(-22, -2),
            S(-20, -42), S(43, -20), S(-33, 0), S(-96, 24), S(-87, 45), S(9, 51), S(22, 40), S(22, -53)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "PST_FILE",
        .data = {
            S(-21, 15), S(-9, 14), S(-5, -3), S(7, -16), S(17, -10), S(12, -1), S(15, 5), S(-15, -5),
            S(-37, -19), S(-14, -3), S(-5, 14), S(15, 18), S(18, 18), S(17, 4), S(6, -4), S(0, -27),
            S(-17, -6), S(2, 0), S(3, -1), S(-3, 11), S(5, 8), S(-3, 8), S(11, -2), S(0, -18),
            S(-14, 6), S(-15, 11), S(-7, 14), S(4, 1), S(16, -10), S(4, -3), S(17, -10), S(-5, -10),
            S(-15, -19), S(-12, -5), S(-11, 11), S(-2, 13), S(-1, 19), S(5, 5), S(18, -9), S(18, -15),
            S(8, -62), S(37, -14), S(-2, 15), S(-58, 35), S(-25, 24), S(-46, 26), S(28, -5), S(18, -57)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "MOBILITY",
        .data = {
            S(10, 6), S(8, 7), S(3, 6), S(2, 11), S(-9, -4)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "PASSER",
        .data = {
            S(13, -28), S(10, 6), S(2, 69), S(17, 131), S(-16, 218), S(28, 216)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PHALANX",
        .data = {
            S(9, 4), S(19, 21), S(34, 38), S(65, 101), S(168, 288), S(153, 279)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "THREAT",
        .data = {
            S(71, 27), S(78, 55), S(97, -2), S(84, -58)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "KING_ATTACK",
        .data = {
            S(11, -10), S(24, -5), S(34, -14), S(18, 19)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_PASSER_US",
        .data = {
            S(-54, 44), S(-37, 77), S(-13, 31), S(-23, 9), S(-3, -22), S(6, -38), S(35, -49), S(0, -36)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "KING_PASSER_THEM",
        .data = {
            S(-108, -47), S(17, -79), S(4, -34), S(-15, 2), S(-20, 30), S(-26, 50), S(-45, 70), S(-50, 56)
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