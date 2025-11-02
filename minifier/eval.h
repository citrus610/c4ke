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
        .name = "PST_PAWN",
        .data = {
            S(-61, -20), S(-40, -19), S(-37, -25), S(-32, -33), S(-13, -11), S(4, -16), S(24, -24), S(-22, -41),
            S(-76, -38), S(-58, -40), S(-47, -47), S(-40, -35), S(-31, -35), S(-43, -27), S(-8, -35), S(-30, -46),
            S(-63, -33), S(-47, -30), S(-36, -46), S(-22, -52), S(-10, -47), S(-9, -41), S(-12, -20), S(-18, -39),
            S(-53, -7), S(-31, -18), S(-27, -39), S(-14, -48), S(9, -46), S(13, -35), S(4, -7), S(1, -15),
            S(-14, 22), S(-4, 26), S(30, -24), S(36, -50), S(56, -43), S(94, -9), S(61, 49), S(38, 39),
            S(41, 134), S(63, 115), S(64, 95), S(116, 64), S(120, 81), S(104, 144), S(33, 173), S(-13, 200)
        },
        .scale = 8,
        .index_offset = -8
    },
    Param {
        .name = "PST_RANK",
        .data = {
            S(-26, -28), S(-7, -14), S(3, 4), S(22, 30), S(35, 38), S(63, 7), S(39, -11), S(-129, -24),
            S(0, -17), S(16, -14), S(16, 5), S(12, 6), S(13, 11), S(21, 3), S(-6, -3), S(-73, 11),
            S(5, -25), S(-5, -27), S(-8, -21), S(-17, 0), S(3, 10), S(19, 11), S(3, 30), S(0, 22),
            S(10, -52), S(18, -48), S(4, -7), S(-1, 14), S(-6, 36), S(12, 25), S(-13, 31), S(-24, 0),
            S(-5, -44), S(32, -23), S(-43, 1), S(-105, 26), S(-89, 46), S(12, 49), S(26, 38), S(32, -49)
        },
        .scale = 8,
        .index_offset = -8
    },
    Param {
        .name = "PST_FILE",
        .data = {
            S(-35, -19), S(-13, -4), S(-2, 13), S(15, 19), S(18, 18), S(14, 5), S(5, -4), S(-2, -28),
            S(-17, -5), S(3, 0), S(3, 1), S(-3, 11), S(4, 9), S(-2, 7), S(14, -4), S(-1, -19),
            S(-15, 6), S(-16, 11), S(-8, 14), S(4, 3), S(14, -7), S(4, -1), S(19, -12), S(-3, -13),
            S(-12, -21), S(-9, -10), S(-9, 9), S(-2, 13), S(-2, 21), S(4, 9), S(15, -7), S(15, -14),
            S(10, -56), S(32, -13), S(-8, 15), S(-64, 35), S(-28, 19), S(-44, 20), S(29, -11), S(21, -56)
        },
        .scale = 8,
        .index_offset = -8
    },
    Param {
        .name = "MOBILITY",
        .data = {
            S(9, 6), S(8, 7), S(3, 6), S(1, 11), S(-7, -2)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "PASSER",
        .data = {
            S(1, -21), S(1, 11), S(-4, 73), S(15, 132), S(-11, 229), S(16, 195)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PHALANX",
        .data = {
            S(4, 4), S(24, 23), S(35, 40), S(67, 101), S(170, 290), S(153, 279)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "THREAT",
        .data = {
            S(70, 30), S(79, 57), S(97, 1), S(85, -53)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "KING_ATTACK",
        .data = {
            S(9, -10), S(25, -7), S(31, -14), S(17, 18)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_PASSER_US",
        .data = {
            S(-60, 47), S(-48, 77), S(-20, 33), S(-30, 11), S(-10, -18), S(-2, -30), S(25, -36), S(-8, -25)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "KING_PASSER_THEM",
        .data = {
            S(-109, -52), S(13, -84), S(8, -39), S(-4, -4), S(-4, 25), S(-3, 44), S(-13, 63), S(-21, 50)
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