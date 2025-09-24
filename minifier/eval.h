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
            S(0, 0), S(-9, 25), S(-22, 16), S(-9, 16), S(5, 25), S(61, 56), S(51, 114), S(0, 0),
            S(48, 60), S(67, 69), S(80, 80), S(97, 107), S(110, 111), S(135, 81), S(109, 71), S(-48, 71),
            S(49, 52), S(66, 54), S(70, 69), S(65, 70), S(68, 73), S(77, 65), S(46, 62), S(-20, 80),
            S(64, 118), S(53, 116), S(54, 120), S(45, 144), S(65, 153), S(82, 153), S(66, 166), S(57, 158),
            S(152, 105), S(159, 120), S(147, 151), S(142, 166), S(135, 185), S(150, 181), S(121, 189), S(97, 177),
            S(-20, -59), S(38, -22), S(-35, 5), S(-86, 34), S(-71, 57), S(12, 60), S(18, 42), S(20, -61)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "PST_FILE",
        .data = {
            S(-12, 46), S(0, 41), S(3, 21), S(14, 5), S(25, 10), S(20, 22), S(21, 32), S(-8, 23),
            S(37, 75), S(60, 83), S(70, 94), S(90, 99), S(92, 98), S(92, 84), S(80, 81), S(73, 67),
            S(43, 70), S(62, 72), S(63, 72), S(57, 82), S(65, 80), S(57, 78), S(71, 68), S(59, 58),
            S(28, 139), S(26, 144), S(34, 146), S(44, 133), S(56, 123), S(44, 130), S(57, 123), S(36, 123),
            S(136, 156), S(139, 162), S(141, 167), S(147, 177), S(149, 181), S(152, 181), S(165, 175), S(165, 176),
            S(6, -67), S(32, -11), S(-8, 21), S(-63, 44), S(-30, 32), S(-49, 28), S(25, -7), S(17, -65)
        },
        .scale = 8,
        .index_offset = 0
    },
    Param {
        .name = "MOBILITY",
        .data = {
            S(9, 10), S(8, 7), S(2, 7), S(-1, 24), S(-9, -7)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "PASSER",
        .data = {
            S(-9, 0), S(-12, 17), S(-12, 52), S(11, 85), S(-7, 150), S(16, 184)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "PHALANX",
        .data = {
            S(8, 3), S(19, 18), S(34, 34), S(65, 92), S(147, 214), S(144, 218)
        },
        .scale = 8,
        .index_offset = -1
    },
    Param {
        .name = "THREAT",
        .data = {
            S(70, 25), S(77, 45), S(95, 1), S(81, -45)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_ATTACK",
        .data = {
            S(11, -9), S(23, -5), S(30, -12), S(19, 15)
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