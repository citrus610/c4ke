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
    "vector",
    "abs",
    "istream",
    "memset",
    "string",
    "isdigit",
    "stoi",
    "max",
    "min",
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

    // Init all 1 letter, 2 letters names and 3 letters names
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

    for (size_t i = 0; i < FIRST_CHAR_COUNT; ++i) {
        for (size_t k = 0; k < REMAIN_CHAR_COUNT; ++k) {
            for (size_t n = 0; n < REMAIN_CHAR_COUNT; ++n) {
                std::string name;

                name += CHARACTERS[i];
                name += CHARACTERS[k];
                name += CHARACTERS[n];

                NAMES.push_back(name);
            }
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

// Rename variable and struct
std::vector<std::string> get_renamed(std::vector<std::string> tokens)
{
    // Name and frequency map
    struct Name
    {
        std::string name;
        int count;
    };

    std::vector<Name> names;

    // Iterate all tokens
    bool is_in_string = false;

    for (auto& token : tokens) {
        // Check if in string
        if (token == "\"" || token == "\'") {
            is_in_string = !is_in_string;
        }

        // Skip non word or if in string
        if (!is_word_character(token.front()) || isdigit(token.front()) || is_in_string) {
            continue;
        }

        // Skip special keyword
        if (std::find(KEYWORDS.begin(), KEYWORDS.end(), token) != KEYWORDS.end()) {
            continue;
        }

        if (std::find(KEYWORDS_SKIP.begin(), KEYWORDS_SKIP.end(), token) != KEYWORDS_SKIP.end()) {
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
            names.push_back(Name {
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
            [] (const Name& a, const Name& b) {
                return a.count > b.count;
            }
        );
    }

    // Iterate all tokens
    is_in_string = false;

    for (auto& token : tokens) {
        // Check if in string
        if (token == "\"" || token == "\'") {
            is_in_string = !is_in_string;
        }

        // Skip non word or if in string
        if (!is_word_character(token.front()) || is_in_string) {
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

    // Rename
    tokens = get_renamed(tokens);

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