#include <bits/stdc++.h>

// Output file name
const std::string MINI_FILE = "mini.cpp";

// List of keywords
const std::vector<std::string> KEYWORDS = {
    "alignas",
    "alignof",
    "and",
    "and_eq",
    "asm",
    "auto",
    "bitand",
    "bitor",
    "bool",
    "break",
    "case",
    "catch",
    "char",
    "char8_t",
    "char16_t",
    "char32_t",
    "class",
    "compl",
    "concept",
    "const",
    "consteval",
    "constexpr",
    "constinit",
    "const_cast",
    "continue",
    "co_await",
    "co_return",
    "co_yield",
    "decltype",
    "default",
    "delete",
    "do",
    "double",
    "dynamic_cast",
    "else",
    "enum",
    "explicit",
    "export",
    "extern",
    "false",
    "final",
    "float",
    "for",
    "friend",
    "goto",
    "if",
    "inline",
    "int",
    "int8_t",
    "int16_t",
    "int32_t",
    "int64_t",
    "long",
    "mutable",
    "namespace",
    "new",
    "noexcept",
    "not",
    "not_eq",
    "nullptr",
    "operator",
    "or",
    "or_eq",
    "private",
    "protected",
    "public",
    "register",
    "reinterpret_cast",
    "requires",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "static_assert",
    "static_cast",
    "struct",
    "switch",
    "template",
    "this",
    "thread_local",
    "throw",
    "true",
    "try",
    "typedef",
    "typeid",
    "typename",
    "uint8_t",
    "uint16_t",
    "uint32_t",
    "uint64_t",
    "union",
    "unsigned",
    "using",
    "virtual",
    "void",
    "volatile",
    "wchar_t",
    "while",
    "xor",
    "xor_eq"
};

// List of special keywords that shouldn't be renamed
// This is a bad way to rename variables and structs but I'm not smart enough to implement a whole AST
const std::vector<std::string> KEYWORDS_SKIP = {
    "std",
    "timespec",
    "clock_gettime",
    "tv_sec",
    "tv_nsec",
    "cout",
    "put",
    "endl",
    "__builtin_ctzll",
    "__builtin_popcountll",
    "__builtin_bswap64",
    "vector",
    "abs",
    "istream",
    "memset",
    "memcpy",
    "string",
    "isdigit",
    "stoi",
    "max",
    "min",
    "clamp",
    "swap",
    "push_back",
    "pop_back",
    "size",
    "stringstream",
    "UINT64_MAX",
    "main",
    "argc",
    "argv",
    "mt19937_64",
    "log",
    "calloc",
    "cin",
    "getline",
    "clear",
    "thread",
    "join",
    "free"
};

// Check if is a word character
bool is_word_character(char c)
{
    if (c >= 'a' && c <= 'z') {
        return true;
    }

    if (c >= 'A' && c <= 'Z') {
        return true;
    }

    if (c >= '0' && c <= '9') {
        return true;
    }

    return c == '_';
};

// Check if is a white space
bool is_space(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
};

// Get name
std::string get_name(size_t index)
{
    // Init all usable character
    std::string CHARACTERS;

    for (int i = 0; i < 26; i++) {
        CHARACTERS.push_back('a' + i);
    }

    for (int i = 0; i < 26; i++) {
        CHARACTERS.push_back('A' + i);
    }

    CHARACTERS.push_back('_');

    for (int i = 0; i < 10; i++) {
        CHARACTERS.push_back('0' + i);
    }

    const size_t FIRST_CHAR_COUNT = 26 + 26 + 1;
    const size_t REMAIN_CHAR_COUNT = 26 + 26 + 1 + 10;

    // Init all 1 letter, 2 letters names
    std::vector<std::string> NAMES;

    for (size_t i = 0; i < FIRST_CHAR_COUNT; ++i) {
        NAMES.push_back(std::string { CHARACTERS[i] });
    }

    for (size_t i = 0; i < FIRST_CHAR_COUNT; ++i) {
        for (size_t k = 0; k < REMAIN_CHAR_COUNT; ++k) {
            std::string name;

            name += CHARACTERS[i];
            name += CHARACTERS[k];

            NAMES.push_back(name);
        }
    }

    return NAMES[index];
};

// Unifiy all the source code files into one
std::string get_unified()
{
    std::string result;
    
    // Files to be unified in order
    const std::string FILES[] = {
        "chess.cpp",
        "eval.cpp",
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
            auto found = line.find("#include");

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

// Replace scores S(MG, EG)
std::string get_replaced_scores(std::string str)
{
    std::string result;

    std::stringstream ss(str);
    std::string line;

    // Find all instances of S(MG, EG)
    while (std::getline(ss, line)) {
        // Find define
        auto found_define = line.find("#define S(MG, EG)");

        if (found_define != std::string::npos) {
            continue;
        }

        // Create new line
        std::string new_line;
        size_t position = 0;
        size_t position_old = 0;

        // Loop through the line
        while (true) {
            // Find S(MG, EG)
            position_old = position;
            position = line.find("S(", position);

            if (position == std::string::npos) {
                break;
            }

            // Get midgame value
            std::string str_mg;
            size_t index = position + 2;

            for (index; index < line.size(); index++) {
                if (line[index] == ',') {
                    break;
                }

                if (is_space(line[index])) {
                    continue;
                }

                str_mg.push_back(line[index]);
            }

            // Get endgame value
            std::string str_eg;
            index = index + 1;

            for (index; index < line.size(); index++) {
                if (line[index] == ')') {
                    break;
                }

                if (is_space(line[index])) {
                    continue;
                }

                str_eg.push_back(line[index]);
            }

            // Calculate int literal
            int mg = std::stoi(str_mg);
            int eg = std::stoi(str_eg);
            int value = mg + (eg * 0x10000);

            std::string str_value = std::to_string(value);

            // Add to new line
            new_line.append(line, position_old, position - position_old);
            new_line += str_value;
            position = index + 1;
        }

        new_line.append(line, position_old, line.size() - position_old);

        result += new_line;
        result += "\n";
    }

    return result;
}

// Replace constants
struct Constant
{
    std::string name;
    std::string value;
};

// Get all constants
std::vector<Constant> get_constants(std::string str)
{
    std::vector<Constant> result;

    std::stringstream ss(str);
    std::string line;

    // Find all constants
    while (std::getline(ss, line)) {
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

        result.push_back(Constant {
            .name = name,
            .value = value
        });
    }

    return result;
};

// Strip constants define
std::string get_removed_define(std::string str)
{
    std::string result;

    std::stringstream ss(str);
    std::string line;

    // Find all constants
    while (std::getline(ss, line)) {
        // Find declaration
        auto found_define = line.find("#define ");

        if (found_define != std::string::npos) {
            continue;
        }

        // Push
        result += line;
        result += "\n";
    }

    return result;
};

// Get all the tokens
std::vector<std::string> get_tokens(std::string str)
{
    std::vector<std::string> tokens;

    // Iterate lines
    std::stringstream ss(str);
    std::string line;

    while (std::getline(ss, line)) {
        // Find word token
        std::string word_token;

        for (auto character : line) {
            // If the character is a word character
            if (is_word_character(character)) {
                word_token.push_back(character);
                continue;
            }

            // Push the word token to the list if exist
            if (!word_token.empty()) {
                tokens.push_back(word_token);
            }
            
            // Reset the word token
            word_token.clear();

            // Push non word token to the list
            tokens.push_back(std::string { character });
        }

        // Push the word token to the list if exist
        if (!word_token.empty()) {
            tokens.push_back(word_token);
        }

        // Add new line token
        tokens.push_back("\n");
    }

    return tokens;
};

// Get replace constants
std::vector<std::string> get_replaced_constants(std::vector<std::string> tokens, std::vector<Constant> constants)
{
    for (auto& token : tokens) {
        for (auto& constant : constants) {
            if (token == constant.name) {
                token = constant.value;
            }
        }
    }

    return tokens;
};

// Remove white spaces
std::vector<std::string> get_removed_spaces(std::vector<std::string> tokens)
{
    // Remove consecutive white spaces
    std::vector<std::string> result;

    bool is_in_string = false;

    for (size_t i = 0; i < tokens.size(); i++) {
        // Check if is in string
        if (tokens[i] == "\"") {
            is_in_string = !is_in_string;
        }

        // Skip double spaces if not in string
        if (!is_in_string && i + 1 < tokens.size() && is_space(tokens[i].front()) && is_space(tokens[i + 1].front())) {
            continue;
        }

        // Push token
        result.push_back(tokens[i]);
    }

    // Remove white spaces
    tokens = result;

    result.clear();
    is_in_string = false;

    for (size_t i = 0; i < tokens.size(); i++) {
        // Check if we're in string
        if (tokens[i] == "\"") {
            is_in_string = !is_in_string;
        }

        // Skip if not white space or we're in string
        if (!is_space(tokens[i].front()) || is_in_string) {
            result.push_back(tokens[i]);
            continue;
        }

        // Skip whitespace at the start and end of the list (even though we removed them, but just to be safe)
        if (i <= 0 || i >= tokens.size() - 1) {
            continue;
        }

        // If the previous token is a word, and the next token is also a word, then keep this white space
        bool is_before_word = is_word_character(tokens[i - 1].front());
        bool is_after_word = is_word_character(tokens[i + 1].front());

        if (is_before_word && is_after_word) {
            result.push_back(" ");
        }
    }

    return result;
};

// Get custom types defined by user
std::vector<std::string> get_custom_types(std::vector<std::string> tokens)
{
    std::vector<std::string> result;

    // Iterate tokens
    for (size_t index = 0; index < tokens.size(); index++) {
        // Get token
        auto& token = tokens[index];

        // Skip non word
        if (!is_word_character(token.front()) || isdigit(token.front())) {
            continue;
        }

        // Find new types
        std::optional<std::string> new_type = {};

        // Case struct
        if (token == "struct") {
            new_type = tokens[index + 2];
        }

        // Case typedef
        if (token == "typedef") {
            new_type = tokens[index + 4];
        }

        // Failed to find new type
        if (!new_type.has_value()) {
            continue;
        }

        // Check if exist
        bool found = false;

        for (auto& type : result) {
            if (type == new_type.value()) {
                found = true;
                break;
            }
        }

        if (!found) {
            result.push_back(new_type.value());
        }
    }

    return result;
};

// Map name to frequency
struct NameFreq
{
    std::string name;
    size_t count;
};

// Map name to IR
struct NameIR
{
    std::string name;
    std::string ir;
};

// Check if a token is a name
bool is_name(std::string token)
{
    return is_word_character(token.front()) && !isdigit(token.front());
};

// Check if it's in a list
bool is_in_list(const std::vector<std::string>& list, std::string token)
{
    return std::find(list.begin(), list.end(), token) != list.end();
};

bool is_in_list(const std::vector<NameFreq>& list, std::string token)
{
    for (auto& i : list) {
        if (i.name == token) {
            return true;
        }
    }

    return false;
};

bool is_in_list(const std::vector<NameIR>& list, std::string token)
{
    for (auto& i : list) {
        if (i.name == token) {
            return true;
        }
    }

    return false;
};

// Check if it's a function
bool is_function(const std::vector<std::string>& tokens, size_t index)
{
    // bool is_identifier_before = index > 1 && is_name(tokens[index - 2]);
    bool is_parentheses_after = index + 1 < tokens.size() && tokens[index + 1] == "(";

    // if (!is_identifier_before || !is_parentheses_after) {
    if (!is_parentheses_after) {
        return false;
    }

    int parenth_scope = 0;

    for (size_t i = index; i + 1 < tokens.size(); i++) {
        if (tokens[i] == "(") {
            parenth_scope += 1;
        }

        if (tokens[i] == ")") {
            parenth_scope -= 1;

            if (parenth_scope <= 0 && tokens[i + 1] == "{") {
                return true;
            }
        }
    }

    return false;
};

// Convert arguments and variables in function to IRs
size_t convert_function(std::vector<std::string>& tokens, std::vector<NameIR> globals, size_t start)
{
    std::vector<NameFreq> arguments_freq;
    std::vector<NameFreq> variables_freq;

    int counter_scope = 0;
    int parenth_score = 0;
    bool is_string = false;
    bool is_inside = false;

    size_t end = tokens.size();

    // Collect stats
    for (size_t i = start; i < tokens.size(); i++) {
        auto& token = tokens[i];

        // Check if in string
        if (token == "\"" || token == "\'") {
            is_string = !is_string;
        }

        if (is_string) {
            continue;
        }

        // Check if inside the function body
        if (token == "(") {
            parenth_score += 1;
        }

        if (token == ")") {
            parenth_score -= 1;

            if (parenth_score <= 0) {
                is_inside = true;
            }
        }

        // Check scope
        if (token == "{") {
            counter_scope += 1;
        }

        if (token == "}") {
            counter_scope -= 1;

            if (counter_scope <= 0) {
                end = i;
                break;
            }
        }

        // Skip non word
        if (!is_name(token)) {
            continue;
        }

        // Skip special keyword
        if (is_in_list(KEYWORDS, token) || is_in_list(KEYWORDS_SKIP, token)) {
            continue;
        }

        // Skip if this is a global variable
        if (is_in_list(globals, token)) {
            continue;
        }

        // Collect arguments
        if (!is_inside) {
            arguments_freq.push_back(NameFreq {
                .name = token,
                .count = 1
            });

            continue;
        }

        // Argument
        bool is_argument = false;

        for (auto& arg : arguments_freq) {
            if (arg.name == token) {
                arg.count += 1;
                is_argument = true;
                break;
            }
        }

        if (is_argument) {
            continue;
        }

        // Variable
        variables_freq.push_back(NameFreq {
            .name = token,
            .count = 1
        });
    }

    // Error
    if (end >= tokens.size()) {
        std::cout << "ERROR: function rename at " << start << std::endl;

        return start;
    }

    // Sort
    if (!arguments_freq.empty()) {
        std::sort(
            arguments_freq.begin(),
            arguments_freq.end(),
            [] (const NameFreq& a, const NameFreq& b) {
                return a.count > b.count;
            }
        );
    }

    if (!variables_freq.empty()) {
        std::sort(
            variables_freq.begin(),
            variables_freq.end(),
            [] (const NameFreq& a, const NameFreq& b) {
                return a.count > b.count;
            }
        );
    }

    // Add to list
    std::vector<NameIR> arguments;
    std::vector<NameIR> variables;

    for (size_t i = 0; i < arguments_freq.size(); i++) {
        arguments.push_back(NameIR {
            .name = arguments_freq[i].name,
            .ir = std::string("arg_") + std::to_string(i)
        }); 
    }

    for (size_t i = 0; i < variables_freq.size(); i++) {
        variables.push_back(NameIR {
            .name = variables_freq[i].name,
            .ir = std::string("var_") + std::to_string(i)
        }); 
    }

    // Rename
    is_string = false;

    for (size_t i = start; i < end; i++) {
        auto& token = tokens[i];

        // Check if in string
        if (token == "\"" || token == "\'") {
            is_string = !is_string;
        }

        if (is_string) {
            continue;
        }

        // Skip non word
        if (!is_name(token)) {
            continue;
        }

        // Skip special keyword
        if (is_in_list(KEYWORDS, token) || is_in_list(KEYWORDS_SKIP, token)) {
            continue;
        }

        // Global name
        if (is_in_list(globals, token)) {
            for (auto& g : globals) {
                if (g.name == token) {
                    token = g.ir;
                    break;
                }
            }
        }
        // Argument
        else if (is_in_list(arguments, token)) {
            for (auto& a : arguments) {
                if (a.name == token) {
                    token = a.ir;
                    break;
                }
            }
        }
        // Variable
        else if (is_in_list(variables, token)) {
            for (auto& v : variables) {
                if (v.name == token) {
                    token = v.ir;
                    break;
                }
            }
        }
    }

    return end;
};

void convert_global(std::vector<std::string>& tokens)
{
    std::vector<NameIR> globals;

    bool is_string = false;

    for (size_t i = 0; i < tokens.size(); i++) {
        // Get token
        auto& token = tokens[i];

        // Check if in string
        if (token == "\"" || token == "\'") {
            is_string = !is_string;
        }

        if (is_string) {
            continue;
        }

        // Skip non word
        if (!is_name(token)) {
            continue;
        }

        // Skip special keyword except "main"
        if ((is_in_list(KEYWORDS, token) || is_in_list(KEYWORDS_SKIP, token)) && token != "main") {
            continue;
        }

        // Don't push new name if this is "main"
        if (token != "main") {
            // Check if this name exist
            bool found = false;

            for (auto& name : globals) {
                if (token == name.name) {
                    token = name.ir;
                    found = true;
                    break;
                }
            }

            // Push new name
            if (!found) {
                auto new_ir = std::string("name_") + std::to_string(globals.size());

                globals.push_back(NameIR {
                    .name = token,
                    .ir = new_ir
                });

                token = new_ir;
            }
        }

        // Check if this is a function
        if (is_function(tokens, i)) {
            i = convert_function(tokens, globals, i + 1);
            continue;
        }
    }
}

// Rename
std::vector<std::string> get_renamed(std::vector<std::string> tokens)
{
    std::vector<NameFreq> names;

    // Iterate all tokens
    bool is_string = false;

    for (auto& token : tokens) {
        // Check if in string
        if (token == "\"" || token == "\'") {
            is_string = !is_string;
        }

        // Skip non word or if in string
        if (!is_name(token) || is_string) {
            continue;
        }

        // Skip special keyword
        if (is_in_list(KEYWORDS, token) || is_in_list(KEYWORDS_SKIP, token)) {
            continue;
        }

        // Update names list
        bool found = false;

        for (auto& name : names) {
            if (name.name == token) {
                name.count += 1;
                found = true;
                break;
            }
        }

        if (!found) {
            names.push_back(NameFreq {
                .name = token,
                .count = 1
            });
        }
    }

    // Sort names based on frequency
    if (!names.empty()) {
        std::sort(
            names.begin(),
            names.end(),
            [] (const NameFreq& a, const NameFreq& b) {
                return a.count > b.count;
            }
        );
    }

    // Iterate all tokens
    is_string = false;

    for (auto& token : tokens) {
        // Check if in string
        if (token == "\"" || token == "\'") {
            is_string = !is_string;
        }

        // Skip non word or if in string
        if (!is_name(token) || is_string) {
            continue;
        }

        // Replace name
        for (size_t i = 0; i < names.size(); ++i) {
            if (names[i].name == token) {
                token = get_name(i);
                break;
            }
        }
    }

    return tokens;
};

int main()
{
    // Create output file
    std::string output;

    output += "#include <bits/stdc++.h>\n";

    // Unify all source code files
    auto str = get_unified();

    // Remove OpenBench codes
    str = get_removed_ob(str);

    // Remove comments
    str = get_removed_comments(str);

    // Replace scores
    str = get_replaced_scores(str);

    // Find all the constants
    auto constants = get_constants(str);

    // Remove define
    str = get_removed_define(str);

    // Get the tokens
    auto tokens = get_tokens(str);

    // Replace constants
    tokens = get_replaced_constants(tokens, constants);

    // Remove white space
    tokens = get_removed_spaces(tokens);

    // Get custom types
    auto custom_types = get_custom_types(tokens);

    // Rename
    convert_global(tokens);

    tokens = get_renamed(tokens);

    // Add to output
    for (auto& token : tokens) {
        output.append(token);
    }

    // Print to stdout
    std::cout << output;

    auto file = std::ofstream("mini.cpp", std::ios::out);

    file << output;
    file.close();

    return 0;
};