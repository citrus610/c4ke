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
            S(0, 0), S(-26, -28), S(-29, -32), S(-23, -33), S(-19, -28), S(-10, -18), S(13, -10), S(0, 0),
            S(-48, -87), S(-41, -82), S(-38, -70), S(-32, -63), S(-32, -57), S(-19, -68), S(-31, -73), S(-80, -67),
            S(-56, -83), S(-48, -83), S(-49, -74), S(-49, -72), S(-48, -70), S(-45, -71), S(-55, -72), S(-75, -67),
            S(-116, -213), S(-122, -210), S(-119, -206), S(-119, -198), S(-114, -192), S(-107, -195), S(-107, -193), S(-101, -197),
            S(-145, -227), S(-141, -226), S(-144, -206), S(-148, -187), S(-145, -185), S(-145, -180), S(-149, -178), S(-150, -188),
            S(-3, -36), S(2, -15), S(-14, -7), S(-14, -2), S(-6, 6), S(19, 15), S(72, 5), S(87, -21)
        },
        .scale = 4,
        .index_offset = 0
    },
    Param {
        .name = "PST_FILE",
        .data = {
            S(-26, -36), S(-19, -35), S(-19, -37), S(-17, -40), S(-18, -39), S(-16, -36), S(-16, -36), S(-27, -38),
            S(-72, -91), S(-65, -82), S(-64, -74), S(-60, -72), S(-59, -72), S(-61, -74), S(-63, -78), S(-68, -86),
            S(-58, -93), S(-56, -87), S(-56, -87), S(-56, -84), S(-55, -85), S(-58, -85), S(-54, -87), S(-56, -94),
            S(-93, -203), S(-88, -203), S(-84, -204), S(-80, -206), S(-79, -207), S(-83, -206), S(-86, -206), S(-89, -208),
            S(-159, -216), S(-159, -209), S(-157, -205), S(-156, -200), S(-154, -205), S(-155, -206), S(-155, -211), S(-158, -210),
            S(5, -15), S(5, 6), S(-6, 11), S(-12, 14), S(-9, 11), S(-8, 11), S(7, 5), S(8, -16)
        },
        .scale = 4,
        .index_offset = 0
    },
    Param {
        .name = "MOBILITY",
        .data = {
            S(3, 0), S(2, 1), S(2, 0), S(2, -2), S(-1, -4)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "PASSER",
        .data = {
            S(-3, 2), S(-7, 8), S(-5, 16), S(1, 21), S(1, 36), S(-22, 60)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "PHALANX",
        .data = {
            S(2, 7), S(5, 9), S(6, 12), S(12, 29), S(65, 60), S(163, 229)
        },
        .scale = 4,
        .index_offset = -1
    },
    Param {
        .name = "THREAT",
        .data = {
            S(30, 28), S(30, 29), S(28, 29), S(28, 21)
        },
        .scale = 1,
        .index_offset = -1
    },
    Param {
        .name = "KING_ATTACK",
        .data = {
            S(4, -4), S(4, -2), S(5, -4), S(3, 3)
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

    for (auto& score : data) {
        result.min_mg = std::min(result.min_mg, get_mg(score));
        result.min_eg = std::min(result.min_eg, get_eg(score));
    }

    for (auto& score : data) {
        int mg = int(std::round(double(get_mg(score) - result.min_mg) / double(scale))) + 32;
        int eg = int(std::round(double(get_eg(score) - result.min_eg) / double(scale))) + 32;

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