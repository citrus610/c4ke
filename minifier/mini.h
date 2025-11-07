#include <bits/stdc++.h>

// Output file name
const inline std::string MINI_FILE = "mini.cpp";

// List of keywords
const inline std::vector<std::string> KEYWORDS = {
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
    "jthread",
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
const inline std::vector<std::string> KEYWORDS_SKIP = {
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
    "__builtin_prefetch",
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
    "free",
    "atomic_int",
    "atomic",
    "mutex",
    "lock_guard"
};

const inline std::vector<std::string> KEYWORDS_STATEMENT = {
    "if",
    "else"
    "for",
    "while"
};

enum class ScopeType
{
    GLOBAL,
    STRUCT,
    FUNCTION,
    STATEMENT,
};

struct Name
{
    std::string name;
    size_t count;
};

struct NameID
{
    std::string name;
    size_t id;
};

struct Scope
{
    ScopeType type;
    std::string name;
    std::vector<Name> names;
    std::vector<Name> toplevels;
    std::vector<Scope> children;
};

struct ScopeIR
{
    ScopeType type;
    std::string name;
    std::vector<NameID> names;
    std::vector<ScopeIR> children;
};

// Check if is a word character
inline bool is_word_character(char c)
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
inline bool is_space(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
};

// Check if we're inside a string literal
inline bool is_in_string(const std::vector<std::string>& tokens, size_t index) {
    return (tokens[index] == "\"" || tokens[index] == "\'") && (index < 1 || tokens[index - 1] != "\\");
}

// Check if a token is a name
inline bool is_name(std::string token)
{
    return is_word_character(token.front()) && !isdigit(token.front());
};

// Check if it's in a list
inline bool is_in_list(const std::vector<std::string>& list, std::string token)
{
    return std::find(list.begin(), list.end(), token) != list.end();
};

inline bool is_in_list(const std::vector<Name>& list, std::string token)
{
    for (auto& i : list) {
        if (i.name == token) {
            return true;
        }
    }

    return false;
};

inline bool is_in_list(const std::vector<NameID>& list, std::string token)
{
    for (auto& i : list) {
        if (i.name == token) {
            return true;
        }
    }

    return false;
};

// Check if it's a field from another struct
inline bool is_field(const std::vector<std::string>& tokens, size_t index) {
    return index > 0 && tokens[index - 1] == ".";
}

// Check if it's a statement
inline bool is_statement(const std::vector<std::string>& tokens, size_t index)
{
    if (!is_in_list(KEYWORDS_STATEMENT, tokens[index])) {
        return false;
    }

    int parenth_scope = 0;

    for (size_t i = index; i + 1 < tokens.size(); i++) {
        if (tokens[i] == "(") {
            parenth_scope += 1;
        }

        if (tokens[i] == ")") {
            parenth_scope -= 1;

            if (parenth_scope <= 0) {
                return tokens[i + 1] == "{";
            }
        }
    }

    return false;
};

// Check if it's a struct
inline bool is_struct(const std::vector<std::string>& tokens, size_t index)
{
    return index > 1 && tokens[index - 2] == "struct";
};

// Check if it's a function
inline bool is_function(const std::vector<std::string>& tokens, size_t index)
{
    bool is_parentheses_after = index + 1 < tokens.size() && tokens[index + 1] == "(";

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

            if (parenth_scope <= 0) {
                return tokens[i + 1] == "{";
            }
        }
    }

    return false;
};

// Get name
inline std::string get_name(size_t index)
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
inline std::string get_unified()
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
inline std::string get_removed_ob(std::string str, bool is_bench)
{
    std::string result;

    // Iterate lines
    std::stringstream ss(str);
    std::string line;

    bool is_ob = false;
    bool is_ob_mini = false;
    bool is_ob_mini_else = false;

    while (std::getline(ss, line)) {
        // Find the OB declaration
        auto found_define = line.find("#define OB");

        if (found_define != std::string::npos) {
            continue;
        }

        // Find the start of OB code
        auto found_ifdef = line.find("#ifdef OB");

        if (found_ifdef != std::string::npos) {
            if (line.find("#ifdef OB_MINI") != std::string::npos && is_bench) {
                is_ob = false;
                is_ob_mini = true;
                is_ob_mini_else = false;
            }
            else {
                is_ob = true;
                is_ob_mini = false;
                is_ob_mini_else = false;
            }

            continue;
        }

        // Find the end of OB code
        bool found_else = line.find("#else") != std::string::npos;
        bool found_endif = line.find("#endif") != std::string::npos;

        // If we don't allow bench
        if (is_ob) {
            if (found_else || found_endif) {
                is_ob = false;
            }

            continue;
        }

        // If we allow bench
        if (is_ob_mini) {
            if (found_else) {
                is_ob_mini_else = true;
                continue;
            }

            if (found_endif) {
                is_ob_mini = false;
                is_ob_mini_else = false;
                continue;
            }

            if (is_ob_mini_else) {
                continue;
            }
        }

        // Skip #else and #endif
        if (found_else || found_endif) {
            continue;
        }

        // Push line to the output string
        result += line;
        result += '\n';
    }

    return result;
};

// Remove comments
inline std::string get_removed_comments(std::string str)
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
inline std::string get_replaced_scores(std::string str)
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
inline std::vector<Constant> get_constants(std::string str, bool is_tcec)
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

        // Find value index
        size_t index = found_define;

        for (int i = 0; i < 2; i++) {
            for (; index < line.size(); index++) {
                if (line[index] == ' ') {
                    index += 1;
                    break;
                }
            }
        }

        // Push constant
        auto line_copy = line;

        std::stringstream ss_line(line_copy);
        std::string name;
        std::string value;

        ss_line >> name >> name;

        value.append(line.begin() + index, line.end());

        while (is_space(value.back())) {
            value.pop_back();
        }

        // Replace values for TCEC build
        // Use 256 threads
        if (is_tcec && name == "THREADS") {
            value = "256";
        }

        // Use 256 GB hash
        if (is_tcec && name == "TT_BITS") {
            value = "35";
        }

        if (is_tcec && name == "TT_SHIFT") {
            value = "29";
        }

        result.push_back(Constant {
            .name = name,
            .value = value
        });
    }

    return result;
};

// Strip constants define
inline std::string get_removed_define(std::string str)
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
inline std::vector<std::string> get_tokens(std::string str)
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
inline std::vector<std::string> get_replaced_constants(std::vector<std::string> tokens, std::vector<Constant> constants)
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
inline std::vector<std::string> get_removed_spaces(std::vector<std::string> tokens)
{
    // Remove consecutive white spaces
    std::vector<std::string> result;

    bool is_string = false;

    for (size_t i = 0; i < tokens.size(); i++) {
        // Check if is in string
        if (is_in_string(tokens, i)) {
            is_string = !is_string;
        }

        // Skip double spaces if not in string
        if (!is_string && i + 1 < tokens.size() && is_space(tokens[i].front()) && is_space(tokens[i + 1].front())) {
            continue;
        }

        // Push token
        result.push_back(tokens[i]);
    }

    // Remove white spaces
    tokens = result;

    result.clear();
    is_string = false;

    for (size_t i = 0; i < tokens.size(); i++) {
        // Check if we're in string
        if (is_in_string(tokens, i)) {
            is_string = !is_string;
        }

        // Skip if not white space or we're in string
        if (!is_space(tokens[i].front()) || is_string) {
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

// Get all the names in the parent scope and this scope
inline std::vector<Name> get_globals(Scope& scope, std::vector<Name> parent_globals)
{
    for (auto& name : scope.names) {
        parent_globals.push_back(name);
    }

    return parent_globals;
};

// Push top level names used in the child scope to this scope
inline void push_child_toplevels(Scope& scope, Scope& child)
{
    for (auto& child_toplevel : child.toplevels) {
        bool found = false;

        for (auto& toplevel : scope.toplevels) {
            if (toplevel.name == child_toplevel.name) {
                toplevel.count += child_toplevel.count;
                found = true;
                break;
            }
        }

        for (auto& name : scope.names) {
            if (name.name == child_toplevel.name) {
                name.count += child_toplevel.count;
                found = true;
                break;
            }
        }

        if (!found) {
            scope.toplevels.push_back(child_toplevel);
        }
    }
};

// Get the scope tree of the c++ source file
inline std::pair<Scope, size_t> get_scope(ScopeType type, std::string name, std::vector<Name> globals, std::vector<Scope>& structs, size_t index, std::vector<std::string> tokens)
{
    Scope scope;
    size_t end;

    scope.type = type;
    scope.name = name;
    scope.names.clear();
    scope.toplevels.clear();
    scope.children.clear();

    int scope_counter = 0;
    bool is_string = false;

    // Iterate token
    for (size_t i = index; i < tokens.size(); ++i) {
        auto& token = tokens[i];

        // Check if in string
        if (is_in_string(tokens, i)) {
            is_string = !is_string;
        }

        if (is_string) {
            continue;
        }

        // Check scope
        if (token == "{") {
            scope_counter += 1;
        }

        if (token == "}") {
            scope_counter -= 1;

            if (scope_counter <= 0 && type != ScopeType::GLOBAL) {
                end = i;
                break;
            }
        }

        // Skip non word or if in string
        if (!is_name(token)) {
            continue;
        }

        // Special check for "if", "else", "for", "while" statements
        bool is_token_statement = is_statement(tokens, i);

        // Skip special keyword except "main" and statements
        if ((is_in_list(KEYWORDS, token) || is_in_list(KEYWORDS_SKIP, token)) && token != "main" && !is_token_statement) {
            continue;
        }

        // Update names list
        if (!is_token_statement) {
            // Check if this is a global name
            if (is_in_list(globals, token)) {
                bool found = false;

                for (auto& toplevel : scope.toplevels) {
                    if (toplevel.name == token) {
                        toplevel.count += 1;
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    scope.toplevels.push_back(Name {
                        .name = token,
                        .count = 1
                    });
                }
            }
            // Check if we've found this name before in this scope
            else if (is_in_list(scope.names, token)) {
                for (auto& variable : scope.names) {
                    if (variable.name == token) {
                        variable.count += 1;
                        break;
                    }
                }
            }
            // Check if this is another struct's field or method
            else if (is_field(tokens, i)) {
                auto& structs_list = type == ScopeType::GLOBAL ? scope.children : structs;

                for (auto& a_struct : structs_list) {
                    if (a_struct.type != ScopeType::STRUCT) {
                        continue;
                    }

                    bool found = false;

                    for (auto& field : a_struct.names) {
                        if (field.name == token) {
                            field.count += 1;
                            found = true;
                            break;
                        }
                    }

                    if (found) {
                        break;
                    }
                }
            }
            // New name
            else {
                scope.names.push_back(Name {
                    .name = token,
                    .count = 1
                });
            }
        }

        // Check if this is a statement: if, for, while
        if (is_token_statement) {
            auto& structs_list = type == ScopeType::GLOBAL ? scope.children : structs;

            auto [child, child_end] = get_scope(ScopeType::STATEMENT, "", get_globals(scope, globals), structs_list, i + 1, tokens);

            push_child_toplevels(scope, child);

            scope.children.push_back(child);

            i = child_end;
        }
        // Check if this is a function
        else if (is_function(tokens, i)) {
            auto& structs_list = type == ScopeType::GLOBAL ? scope.children : structs;

            auto [child, child_end] = get_scope(ScopeType::FUNCTION, token, get_globals(scope, globals), structs_list, i + 1, tokens);

            push_child_toplevels(scope, child);

            scope.children.push_back(child);

            i = child_end;
        }
        // Check if this a struct
        else if (is_struct(tokens, i)) {
            auto& structs_list = type == ScopeType::GLOBAL ? scope.children : structs;

            auto [child, child_end] = get_scope(ScopeType::STRUCT, token, get_globals(scope, globals), structs_list, i + 1, tokens);

            push_child_toplevels(scope, child);

            scope.children.push_back(child);

            i = child_end;
        }
    }

    return { scope, end };
};

// Print the scope tree to a string
std::string print_scope(Scope& scope, int level)
{
    std::string str;

    std::string indent;

    for (int i = 0; i < level; ++i) {
        indent += "    ";
    }

    str += indent + "type: " + std::to_string(int(scope.type)) + "\n";

    str += indent + "name: " + scope.name + "\n";

    str += indent + "names: {\n";

    for (auto& name : scope.names) {
        str += indent + "    " + name.name + ": " + std::to_string(name.count) + "\n";
    }

    str += indent + "}\n";
    
    str += indent + "child: {\n";
    
    for (auto& child : scope.children) {
        str += print_scope(child, level + 1);
    }

    str += indent + "}\n";

    str += indent + "top lvl: {\n";

    for (auto& toplvl : scope.toplevels) {
        str += indent + "    " + toplvl.name + ": " + std::to_string(toplvl.count) + "\n";
    }
    
    str += indent + "}\n\n";

    return str;
};

// Get all the names' id in the scope tree
ScopeIR get_scope_id(Scope& scope, std::vector<NameID> toplevels)
{
    ScopeIR result;

    result.type = scope.type;
    result.name = scope.name;
    result.names.clear();
    result.children.clear();

    // Sort the names of this scope based on how many times they're used
    if (!scope.names.empty()) {
        std::stable_sort(
            scope.names.begin(),
            scope.names.end(),
            [] (const Name& a, const Name& b) {
                return a.count > b.count;
            }
        );
    }

    // Get all the used ids that are used by top level names
    std::vector<size_t> used_ids;

    for (auto& toplvl_used : scope.toplevels) {
        for (auto& toplvl_id : toplevels) {
            if (toplvl_used.name == toplvl_id.name) {
                used_ids.push_back(toplvl_id.id);
                break;
            }
        }
    }

    // Get id
    size_t id = 0;

    for (auto& name : scope.names) {
        // Find new id
        while (true)
        {
            bool found = false;

            // Check if this id is used before
            for (auto used : used_ids) {
                if (used == id) {
                    found = true;
                    id += 1;
                    break;
                }
            }

            if (!found) {
                break;
            }
        }

        // Push new id
        result.names.push_back(NameID {
            .name = name.name,
            .id = id
        });

        // Update new id
        id += 1;
    }

    // Get the child scope top levels
    auto child_toplevels = toplevels;

    for (auto& name : result.names) {
        child_toplevels.push_back(name);
    }

    // Push new child scope
    for (auto& child : scope.children) {
        result.children.push_back(get_scope_id(child, child_toplevels));
    }

    return result;
};

std::string print_scope_id(ScopeIR& scope_id, int level)
{
    std::string str;

    std::string indent;

    for (int i = 0; i < level; ++i) {
        indent += "    ";
    }

    str += indent + "type: " + std::to_string(int(scope_id.type)) + "\n";
    str += indent + "name: " + scope_id.name + "\n";

    str += indent + "names: {\n";

    for (auto& name : scope_id.names) {
        str += indent + "    " + name.name + ": " + std::to_string(name.id) + "\n";
    }

    str += indent + "}\n";
    
    str += indent + "child: {\n";
    
    for (auto& child : scope_id.children) {
        str += print_scope_id(child, level + 1);
    }
    
    str += indent + "}\n\n";

    return str;
};

// Replace names with intermediate representation
size_t get_replace_ir(ScopeIR& scope_id, std::vector<NameID> globals, std::vector<ScopeIR>& structs, std::vector<std::string>& tokens, size_t index)
{
    size_t end;

    // Get all the names from previous scope and this scope
    auto child_globals = globals;

    for (auto& name : scope_id.names) {
        child_globals.push_back(name);
    }

    // Iterate tokens
    int scope_counter = 0;
    int scope_index = 0;
    bool is_string = false;

    for (size_t i = index; i < tokens.size(); ++i) {
        auto& token = tokens[i];

        // Check if in string
        if (is_in_string(tokens, i)) {
            is_string = !is_string;
        }

        if (is_string) {
            continue;
        }

        // Check scope
        if (token == "{") {
            scope_counter += 1;
        }

        if (token == "}") {
            scope_counter -= 1;

            if (scope_counter <= 0 && scope_id.type != ScopeType::GLOBAL) {
                end = i;
                break;
            }
        }

        // Skip non word or if in string
        if (!is_name(token)) {
            continue;
        }

        // Special check for "if", "else", "for", "while" statements
        bool is_token_statement = is_statement(tokens, i);

        // Skip special keyword except "main" or statements
        if ((is_in_list(KEYWORDS, token) || is_in_list(KEYWORDS_SKIP, token)) && token != "main" && !is_token_statement) {
            continue;
        }

        // Replace name
        if (!is_token_statement) {
            // Check if this is another struct's field or method
            if (is_field(tokens, i)) {
                for (auto& a_struct : structs) {
                    if (a_struct.type != ScopeType::STRUCT) {
                        continue;
                    }

                    bool found = false;

                    for (auto& field : a_struct.names) {
                        if (field.name == token) {
                            token = std::string("name_") + std::to_string(field.id);
                            found = true;
                            break;
                        }
                    }

                    if (found) {
                        break;
                    }
                }
            }
            // Check if this is a top level name
            else if (is_in_list(globals, token) && token != "main") {
                for (auto& name : globals) {
                    if (name.name == token) {
                        token = std::string("name_") + std::to_string(name.id);
                        break;
                    }
                }
            }
            // Check if we've found this name before in this scope
            else if (is_in_list(scope_id.names, token) && token != "main") {
                for (auto& name : scope_id.names) {
                    if (name.name == token) {
                        token = std::string("name_") + std::to_string(name.id);
                        break;
                    }
                }
            }
            // New name
            else if (token != "main") {
                std::cout << "ERROR: Unknown indentifier " << token << std::endl;
            }
        }

        // Check if this is a statement: if, for, while
        if (is_token_statement) {
            if (scope_id.children[scope_index].type != ScopeType::STATEMENT) {
                std::cout << "ERROR: Wrong scope type " << int(scope_id.children[scope_index].type) << ", expected " << int(ScopeType::STATEMENT) << std::endl;
            }

            auto child_end = get_replace_ir(scope_id.children[scope_index], child_globals, structs, tokens, i + 1);

            scope_index += 1;
            i = child_end;
        }
        // Check if this is a function
        else if (is_function(tokens, i)) {
            if (scope_id.children[scope_index].type != ScopeType::FUNCTION) {
                std::cout << "ERROR: Wrong scope type " << int(scope_id.children[scope_index].type) << ", expected " << int(ScopeType::FUNCTION) << std::endl;
            }

            auto child_end = get_replace_ir(scope_id.children[scope_index], child_globals, structs, tokens, i + 1);

            scope_index += 1;
            i = child_end;
        }
        // Check if this a struct
        else if (is_struct(tokens, i)) {
            if (scope_id.children[scope_index].type != ScopeType::STRUCT) {
                std::cout << "ERROR: Wrong scope type " << int(scope_id.children[scope_index].type) << ", expected " << int(ScopeType::STRUCT) << std::endl;
            }

            auto child_end = get_replace_ir(scope_id.children[scope_index], child_globals, structs, tokens, i + 1);

            scope_index += 1;
            i = child_end;
        }
    }

    return end;
};

inline std::vector<std::string> get_renamed(std::vector<std::string> tokens)
{
    std::vector<Name> names;

    // Iterate all tokens
    bool is_string = false;

    for (size_t i = 0; i < tokens.size(); i++) {
        auto& token = tokens[i];

        // Check if in string
        if (is_in_string(tokens, i)) {
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
    is_string = false;

    for (size_t i = 0; i < tokens.size(); i++) {
        auto& token = tokens[i];

        // Check if in string
        if (is_in_string(tokens, i)) {
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

inline std::vector<std::string> get_removed_redundant_math(std::vector<std::string> tokens)
{
    std::vector<std::string> result;

    // Iterate all tokens
    bool is_string = false;

    for (size_t i = 0; i < tokens.size(); i++) {
        auto& token = tokens[i];

        // Check if in string
        if (is_in_string(tokens, i)) {
            is_string = !is_string;
        }

        if (!is_string) {
            // Remove "+-"
            if (token == "+" && i + 1 < tokens.size() && tokens[i + 1].front() == '-') {
                continue;
            }

            // Remove "+0" and "-0"
            if ((token == "+" || token == "-") && i + 1 < tokens.size() && tokens[i + 1] == "0" && (i + 2 >= tokens.size() || tokens[i + 2] != ".")) {
                continue;
            }

            if (token == "0" && i > 0 && (tokens[i - 1] == "+" || tokens[i - 1] == "-") && (i + 1 >= tokens.size() || tokens[i + 1] != ".")) {
                continue;
            }
        }

        // Push
        result.push_back(token);
    }

    return result;
};