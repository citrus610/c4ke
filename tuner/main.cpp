#include <bits/stdc++.h>
#include <fcntl.h>
#include <io.h>

#define S(MG, EG) (MG + (EG << 16))

struct Value
{
    std::wstring name;
    std::vector<int> data;
};

auto PST_RANK = Value {
    .name = L"PST_RANK",
    .data = {
        0, S(-24, 0), S(-24, -8), S(-8, -8), S(8, 0), S(40, 24), 0, 0,
        S(-16, -40), S(0, -24), S(8, -8), S(24, 24), S(32, 32), S(40, 8), S(16, 0), S(-120, 8),
        S(0, -16), S(16, -8), S(16, 0), S(16, 0), S(16, 0), S(16, 0), S(-8, 0), S(-80, 16),
        S(0, -24), S(-8, -24), S(-16, -16), S(-16, 0), S(0, 16), S(16, 16), S(8, 24), S(16, 8),
        S(16, -88), S(24, -64), S(16, -24), S(0, 16), S(0, 40), S(-8, 40), S(-32, 56), S(-16, 32),
        S(-8, -48), S(8, -16), S(-8, 0), S(-32, 24), S(-8, 40), S(40, 32), S(40, 16), S(40, -48)
    }
};

auto PST_FILE = Value {
    .name = L"PST_FILE",
    .data = {
        S(-8, 8), S(-16, 8), S(-8, 0), S(0, -8), S(8, 0), S(16, 0), S(16, 0), S(-8, 0),
        S(-32, -24), S(-8, -8), S(0, 8), S(16, 24), S(16, 24), S(16, 0), S(8, -8), S(-8, -24),
        S(-16, -8), 0, S(8, 0), S(0, 8), S(8, 8), S(0, 8), S(16, 0), S(-8, -8),
        S(-16, 0), S(-8, 8), S(0, 8), S(8, 0), S(16, -8), S(8, 0), S(8, 0), S(-8, -8),
        S(-16, -24), S(-8, -8), S(-8, 0), S(0, 8), S(0, 16), S(8, 16), S(16, 0), S(8, -8),
        S(-16, -40), S(16, -8), S(-8, 8), S(-32, 16), S(-32, 16), S(-16, 16), S(16, -8), S(0, -40)
    }
};

auto MOBILITY = Value {
    .name = L"MOBILITY",
    .data = {
        0, S(8, 5), S(7, 7), S(3, 5), S(3, 2), S(-5, -1)
    }
};

auto PAWN_PASSED = Value {
    .name = L"PAWN_PASSED",
    .data = {
        0, S(0, 15), S(-5, 20), S(-10, 50), S(10, 75), S(15, 100), S(25, 150)
    }
};

int get_midgame(int score)
{
    return int16_t(score);
};

int get_endgame(int score)
{
    return (score + 0x8000) >> 16;
};

// Convert eval data to string
std::pair<std::wstring, std::wstring> get_data_string(std::vector<int> data)
{
    std::pair<std::wstring, std::wstring> result;

    for (auto value : data) {
        wchar_t mg = get_midgame(value) + 512;
        wchar_t eg = get_endgame(value) + 512;

        result.first.push_back(mg);
        result.second.push_back(eg);
    }

    return result;
};

std::wstring get_eval_file()
{
    std::wstring str_index;
    std::wstring str_mg;
    std::wstring str_eg;

    int index = 0;

    std::vector<Value> values = {
        PST_RANK,
        PST_FILE,
        MOBILITY,
        PAWN_PASSED
    };

    for (auto& value : values) {
        auto str_data = get_data_string(value.data);

        str_mg += str_data.first;
        str_eg += str_data.second;

        str_index += L"#define INDEX_" + value.name + L" " + std::to_wstring(index) + L"\n";
        index += value.data.size();
    }

    std::wstring result;

    result += L"#define INDEX_ENDGAME " + std::to_wstring(str_mg.size()) + L"\n";

    result += str_index + L"\n\n";

    result += L"int get_data(int index) {\n";
    result += L"    auto data = L\"" + str_mg + str_eg + L"\";\n\n";
    result += L"    return data[index] + (data[index + INDEX_ENDGAME] << 16) - S(512, 512);\n";
    result += L"}\n";

    return result;
};

int main()
{
    _setmode(_fileno(stdout), _O_U8TEXT);

    auto str = get_eval_file();

    wprintf(L"%ls\n", str.c_str());

    return 0;
};