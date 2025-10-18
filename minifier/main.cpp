#include "mini.h"
#include "eval.h"

int main(int argc, char *argv[])
{
    // Replace eval.cpp data
    if (argc > 1 && std::string(argv[1]) == "eval") {
        std::ifstream in("src/eval.cpp");

        std::string line;
        std::string str;

        while (std::getline(in, line)) {
            if (line.find("#define DATA_STR") != std::string::npos) {
                break;
            }

            str += line + "\n";
        }

        in.close();

        std::ofstream out("src/eval.cpp");

        out << str + get_eval_str();

        out.close();

        return 0;
    }

    // Check if we allow bench in minified code
    bool is_bench = argc > 1 && std::string(argv[1]) == "bench";

    // Create output file
    std::string output;

    output += "#include <bits/stdc++.h>\n";

    // Unify all source code files
    auto str = get_unified();

    // Remove OpenBench codes
    str = get_removed_ob(str, is_bench);

    // Replace scores
    str = get_replaced_scores(str);

    // Find all the constants
    auto constants = get_constants(str, is_bench);

    // Remove define
    str = get_removed_define(str);

    // Remove comments
    str = get_removed_comments(str);

    // Get the tokens
    auto tokens = get_tokens(str);

    // Replace constants
    tokens = get_replaced_constants(tokens, constants);

    // Remove white space
    tokens = get_removed_spaces(tokens);

    // Get scopes stats
    std::vector<Scope> dummy;
    auto global_scope = get_scope(ScopeType::GLOBAL, "global", {}, dummy, 0, tokens).first;
    
    // For debuging purpose
    // auto stat_str = print_scope(global_scope, 0);
    // auto file_stat = std::ofstream("stat.txt", std::ios::out);
    // file_stat << stat_str;
    // file_stat.close();

    // Transform from stats to names' ids
    auto scope_id = get_scope_id(global_scope, {});
    
    // For debuging purpose
    // auto id_str = print_scope_id(scope_id, 0);
    // auto file_id = std::ofstream("id.txt", std::ios::out);
    // file_id << id_str;
    // file_id.close();

    // Replace with ir names
    auto end = get_replace_ir(scope_id, {}, scope_id.children, tokens, 0);

    // Rename
    tokens = get_renamed(tokens);

    // Remove redundant math stuffs
    tokens = get_removed_redundant_math(tokens);

    // Add to output
    for (auto& token : tokens) {
        output.append(token);
    }

    // Print to stdout
    if (!is_bench) {
        std::cout << output;
    }

    auto file = std::ofstream("mini.cpp", std::ios::out);

    file << output;
    file.close();

    return 0;
};