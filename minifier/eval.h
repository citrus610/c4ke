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
            S(-15, 70), S(-1, 73), S(-4, 63), S(1, 47), S(13, 71), S(12, 68), S(20, 65), S(-23, 50),
            S(-29, 57), S(-21, 55), S(-16, 44), S(-12, 47), S(2, 48), S(-26, 54), S(-4, 51), S(-27, 41),
            S(-18, 67), S(-11, 68), S(-3, 43), S(11, 33), S(18, 34), S(19, 41), S(7, 58), S(-2, 42),
            S(-9, 97), S(5, 81), S(5, 57), S(16, 31), S(42, 34), S(43, 40), S(23, 73), S(16, 64),
            S(30, 131), S(31, 136), S(67, 71), S(83, 17), S(101, 12), S(137, 41), S(91, 106), S(53, 108),
            S(92, 226), S(105, 198), S(91, 195), S(153, 113), S(122, 119), S(105, 134), S(-20, 213), S(-57, 246)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "PST_RANK",
        .data = {
            S(53, 92), S(71, 106), S(83, 123), S(101, 148), S(114, 158), S(140, 128), S(115, 111), S(-46, 99),
            S(57, 84), S(74, 86), S(75, 104), S(71, 106), S(72, 112), S(80, 103), S(51, 98), S(-15, 113),
            S(70, 178), S(58, 176), S(57, 181), S(48, 202), S(69, 211), S(85, 212), S(67, 231), S(55, 226),
            S(244, 231), S(251, 238), S(238, 276), S(233, 293), S(227, 314), S(245, 304), S(216, 312), S(192, 301),
            S(-25, -58), S(48, -28), S(-35, 6), S(-94, 38), S(-82, 62), S(8, 63), S(12, 43), S(16, -64)
        },
        .scale = 8,
        .index_offset = -8
    },
    Param {
        .name = "PST_FILE",
        .data = {
            S(44, 105), S(66, 120), S(77, 137), S(94, 142), S(97, 142), S(92, 130), S(84, 121), S(76, 100),
            S(49, 100), S(68, 105), S(68, 106), S(62, 117), S(70, 116), S(62, 114), S(77, 103), S(63, 90),
            S(25, 201), S(25, 206), S(33, 210), S(42, 199), S(53, 190), S(40, 196), S(56, 186), S(36, 183),
            S(226, 274), S(228, 284), S(229, 299), S(235, 307), S(235, 314), S(238, 308), S(251, 295), S(251, 292),
            S(0, -66), S(29, -12), S(-14, 21), S(-66, 44), S(-33, 32), S(-52, 29), S(24, -9), S(13, -63)
        },
        .scale = 8,
        .index_offset = -8
    },
    Param {
        .name = "MOBILITY",
        .data = {
            S(9, 7), S(8, 7), S(2, 6), S(0, 16), S(-11, -3)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "PASSER",
        .data = {
            S(-8, 7), S(-12, 25), S(-13, 64), S(14, 103), S(-1, 197), S(20, 193)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PHALANX",
        .data = {
            S(4, 3), S(24, 22), S(34, 38), S(66, 94), S(168, 267), S(153, 280)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "THREAT",
        .data = {
            S(70, 28), S(78, 54), S(96, -2), S(85, -63)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "KING_ATTACK",
        .data = {
            S(11, -11), S(25, -8), S(33, -14), S(18, 17)
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

    for (auto c : mg) {
        if (c == '\\' || c == '\"') {
            result += "\\";
        }

        result += c;
    }

    for (auto c : eg) {
        if (c == '\\' || c == '\"') {
            result += "\\";
        }

        result += c;
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