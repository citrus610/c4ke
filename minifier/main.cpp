#include <bits/stdc++.h>

const auto MINI_FILE = "mini.cpp";

// Unifiy all the source code files into one
std::string get_unified()
{
    std::string result;
    
    // Files to be unified in order
    const std::string FILES[] = {
        "chess.cpp",
        "board.cpp",
        "search.cpp",
        "main.cpp"
    };

    // Iterate over the files
    for (auto& file_name : FILES) {
        // Open the file
        auto file = std::ifstream(std::string("src/") + file_name);

        // Read each line
        std::string line;

        while (std::getline(file, line)) {
            // Skip line if it's including another file
            auto found = line.find("#include \"");

            if (found != std::string::npos) {
                continue;
            }

            // Push line to the output string
            result += line;
            result += '\n';
        }
    }

    return result;
};

// Remove the OB code
std::string get_removed_ob(std::string str)
{
    std::string result;

    // Iterate lines
    std::stringstream ss(str);
    std::string line;

    bool is_ob = false;

    while (std::getline(ss, line)) {
        // Find the OB declaration
        auto found_define = line.find("#define OB");

        if (found_define != std::string::npos) {
            continue;
        }

        // Find the start of OB code
        auto found_ifdef = line.find("#ifdef OB");

        if (found_ifdef != std::string::npos) {
            is_ob = true;
            continue;
        }

        // Find the end of OB code
        auto found_else = line.find("#else");
        auto found_endif = line.find("#endif");

        if (found_else != std::string::npos || found_endif != std::string::npos) {
            is_ob = false;
            continue;
        }

        // Skip OB codes
        if (is_ob) {
            continue;
        }

        // Push line to the output string
        result += line;
        result += '\n';
    }

    return result;
};

// Remove comments
std::string get_removed_comments(std::string str)
{
    std::string result;

    // Iterate lines
    std::stringstream ss(str);
    std::string line;

    while (std::getline(ss, line)) {
        // Found the comment position
        auto found_comment = line.find("//");

        if (found_comment != std::string::npos) {
            // Erase the comment
            line.erase(line.begin() + found_comment, line.end());
        }

        // Push line to the output string
        result += line;
        result += '\n';
    }

    return result;
};

// Replace constants
std::string get_replaced_constants(std::string str)
{
    std::string result;

    std::stringstream ss_pass_1st(str);
    std::stringstream ss_pass_2nd(str);
    std::string line;

    // Find all constants
    struct Constant
    {
        std::string name;
        std::string value;
    };

    std::vector<Constant> list;

    while (std::getline(ss_pass_1st, line)) {
        // Find declaration
        auto found_define = line.find("#define ");

        if (found_define == std::string::npos) {
            continue;
        }

        // Push constant
        std::stringstream ss_line(line);
        std::string name;
        std::string value;

        ss_line >> name >> name;
        ss_line >> value;

        list.push_back(Constant {
            .name = name,
            .value = value
        });
    }

    // Sort the constants list
    // We replace constants with longer name first
    // This way we won't wrongly replace constants that have overlaping name such as PAWN and WHITE_PAWM
    if (!list.empty()) {
        std::sort(
            list.begin(),
            list.end(),
            [] (const Constant& a, const Constant& b) {
                return a.name.size() > b.name.size();
            }
        );
    }

    // Iterate lines
    while (std::getline(ss_pass_2nd, line)) {
        // Remove constant declaration
        auto found_define = line.find("#define ");

        if (found_define != std::string::npos) {
            continue;
        }

        // Iterate the constants list
        for (auto& constant : list) {
            while (true) {
                // Find the position
                auto position = line.find(constant.name);

                // Break if we can't find
                if (position == std::string::npos) {
                    break;
                }

                // Remove the constant name
                line.erase(line.begin() + position, line.begin() + position + constant.name.size());

                // Insert the constant value
                line.insert(position, constant.value);
            }
        }

        // Push line to the output string
        result += line;
        result += '\n';
    }

    return result;
};

int main()
{
    auto output = std::ofstream(MINI_FILE, std::ios::out | std::ios::app);

    auto unified = get_unified();
    auto removed_ob = get_removed_ob(unified);
    auto removed_comments = get_removed_comments(removed_ob);
    auto replaced_constants = get_replaced_constants(removed_comments);

    output << replaced_constants;

    output.close();

    return 0;
};