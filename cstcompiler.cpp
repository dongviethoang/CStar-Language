/*
This is the CStar Compiler.
Yes, it's in C++, for sure.
(and definitely not Rust)
Do not attempt to modify the source. (except me)
How complex is this compiler? Uh... quite complex.
I don't even know where is the parser, if it has one.
No wait, it doesn't have one, it's like an interpreter without a lexer... plus transpiler.
How do I build a lexer, and parser???????
I think I know how. Add keywords into keywords.h, then somehow make the transpiler detach
the strings and parse them into tokens, then parse the tokens into AST, then generate C++ code from AST.
But that is too complex. I'll try tomorrow.

Copyright (c) November 2025 Hoang Viet. All rights reserved.
*/

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <cstdlib>   // for system/getenv
#include "keywords.h"

static std::string trim(const std::string& s);

bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

static std::string stripCstDeclKeyword(const std::string& input) {
    std::string out;
    size_t pos = 0;
    while (pos < input.size()) {
        size_t found = input.find("__cstdecl", pos);
        if (found == std::string::npos) {
            out.append(input, pos, std::string::npos);
            break;
        }
        out.append(input, pos, found - pos);
        pos = found + std::string("__cstdecl").size();
    }
    return trim(out);
}

static std::string trim(const std::string& s) {
    std::string out = s;
    while (!out.empty() && std::isspace(static_cast<unsigned char>(out.front()))) out.erase(out.begin());
    while (!out.empty() && std::isspace(static_cast<unsigned char>(out.back()))) out.pop_back();
    return out;
}

static std::vector<int> makeMiddleFirstOrder(size_t count) {
    std::vector<int> order;
    order.reserve(count);
    if (count == 0) return order;
    if (count == 1) {
        order.push_back(0);
        return order;
    }
    if (count == 2) {
        order = {1, 0};
        return order;
    }
    order.push_back(1);
    order.push_back(0);
    for (size_t i = 2; i < count; ++i) {
        order.push_back(static_cast<int>(i));
    }
    return order;
}

static std::vector<std::string> splitCommaSeparated(const std::string& text) {
    std::vector<std::string> items;
    std::string current;
    for (char ch : text) {
        if (ch == ',') {
            std::string item = trim(current);
            if (!item.empty()) items.push_back(item);
            current.clear();
        } else {
            current += ch;
        }
    }
    std::string item = trim(current);
    if (!item.empty()) items.push_back(item);
    return items;
}

struct ParamSpec {
    std::string type;
    std::string name;
};

enum class TokenKind {
    Identifier,
    Number,
    Keyword,
    Symbol,
    EndOfFile
};

struct Token {
    TokenKind kind;
    std::string text;
};

struct FunctionAst {
    std::string returnType;
    std::string name;
    std::vector<ParamSpec> params;
};

struct CstDeclFunction : FunctionAst {
    std::vector<std::string> body;
};

struct FunctionNode {
    std::string kind;
    std::string returnType;
    std::string name;
    std::vector<ParamSpec> params;
    std::vector<std::string> body;
};

struct SourceAst {
    std::vector<std::string> includes;
    std::vector<FunctionNode> functions;
    std::vector<std::string> statements;
};

static std::vector<Token> lexCStar(const std::string& source) {
    std::vector<Token> tokens;
    for (size_t i = 0; i < source.size();) {
        const char ch = source[i];
        if (std::isspace(static_cast<unsigned char>(ch))) {
            ++i;
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(ch)) || ch == '_') {
            size_t j = i + 1;
            while (j < source.size() && (std::isalnum(static_cast<unsigned char>(source[j])) || source[j] == '_' || source[j] == ':')) {
                ++j;
            }
            std::string text = source.substr(i, j - i);
            tokens.push_back({text == "__cstdecl" ? TokenKind::Keyword : TokenKind::Identifier, text});
            i = j;
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(ch))) {
            size_t j = i + 1;
            while (j < source.size() && std::isdigit(static_cast<unsigned char>(source[j]))) {
                ++j;
            }
            tokens.push_back({TokenKind::Number, source.substr(i, j - i)});
            i = j;
            continue;
        }

        if (ch == '(' || ch == ')' || ch == ',' || ch == '{' || ch == '}' || ch == ';') {
            tokens.push_back({TokenKind::Symbol, std::string(1, ch)});
            ++i;
            continue;
        }

        tokens.push_back({TokenKind::Symbol, std::string(1, ch)});
        ++i;
    }

    tokens.push_back({TokenKind::EndOfFile, ""});
    return tokens;
}

static std::string joinTokenText(const std::vector<Token>& tokens, size_t start, size_t end) {
    std::string out;
    for (size_t i = start; i < end; ++i) {
        if (!out.empty()) out += " ";
        out += tokens[i].text;
    }
    return out;
}

static std::vector<ParamSpec> parseParamListFromTokens(const std::vector<Token>& tokens, size_t& pos) {
    std::vector<ParamSpec> params;
    std::vector<Token> current;

    while (pos < tokens.size() && !(tokens[pos].kind == TokenKind::Symbol && tokens[pos].text == ")")) {
        if (tokens[pos].kind == TokenKind::Symbol && tokens[pos].text == ",") {
            std::string item = trim(joinTokenText(current, 0, current.size()));
            if (!item.empty()) {
                const size_t lastSpace = item.find_last_of(' ');
                ParamSpec param;
                if (lastSpace != std::string::npos) {
                    param.type = trim(item.substr(0, lastSpace));
                    param.name = trim(item.substr(lastSpace + 1));
                } else {
                    param.type = "auto";
                    param.name = item;
                }
                params.push_back(param);
            }
            current.clear();
            ++pos;
            continue;
        }

        current.push_back(tokens[pos]);
        ++pos;
    }

    std::string item = trim(joinTokenText(current, 0, current.size()));
    if (!item.empty()) {
        const size_t lastSpace = item.find_last_of(' ');
        ParamSpec param;
        if (lastSpace != std::string::npos) {
            param.type = trim(item.substr(0, lastSpace));
            param.name = trim(item.substr(lastSpace + 1));
        } else {
            param.type = "auto";
            param.name = item;
        }
        params.push_back(param);
    }

    return params;
}

static CstDeclFunction parseCstDeclSignature(const std::string& source) {
    CstDeclFunction fn;
    std::vector<Token> tokens = lexCStar(source);
    size_t pos = 0;

    std::vector<Token> signatureTokens;
    while (pos < tokens.size() && !(tokens[pos].kind == TokenKind::Symbol && tokens[pos].text == "(")) {
        if (tokens[pos].text != "__cstdecl") {
            signatureTokens.push_back(tokens[pos]);
        }
        ++pos;
    }
    if (pos >= tokens.size()) {
        return fn;
    }

    std::string signature = trim(joinTokenText(signatureTokens, 0, signatureTokens.size()));
    const size_t lastSpace = signature.find_last_of(' ');
    if (lastSpace != std::string::npos) {
        fn.returnType = trim(signature.substr(0, lastSpace));
        fn.name = trim(signature.substr(lastSpace + 1));
    } else {
        fn.returnType = "auto";
        fn.name = trim(signature);
    }

    // skip the opening parenthesis
    if (pos < tokens.size() && tokens[pos].text == "(") {
        ++pos;
    }

    fn.params = parseParamListFromTokens(tokens, pos);
    if (pos < tokens.size() && tokens[pos].text == ")") {
        ++pos;
    }

    return fn;
}

static std::string renderCstDeclLowering(const CstDeclFunction& fn) {
    std::vector<int> order = makeMiddleFirstOrder(fn.params.size());
    std::string implName = fn.name + "_impl";
    std::string implDecl = fn.returnType + " " + implName + "(";
    std::string wrapperDecl = fn.returnType + " " + fn.name + "(";

    for (size_t i = 0; i < fn.params.size(); ++i) {
        if (i > 0) {
            implDecl += ", ";
            wrapperDecl += ", ";
        }
        const size_t index = static_cast<size_t>(order[i]);
        implDecl += fn.params[index].type + " " + fn.params[index].name;
        wrapperDecl += fn.params[i].type + " " + fn.params[i].name;
    }

    implDecl += ") {\n";
    for (const auto& bodyLine : fn.body) {
        implDecl += "    " + bodyLine + "\n";
    }
    implDecl += "}\n\n";

    wrapperDecl += ") {\n    return " + implName + "(";
    for (size_t i = 0; i < order.size(); ++i) {
        if (i > 0) wrapperDecl += ", ";
        wrapperDecl += fn.params[order[i]].name;
    }
    wrapperDecl += ");\n}\n\n";

    return implDecl + wrapperDecl;
}

std::string current_ver = "CStar26 Debug 3";

struct ParsedSource {
    std::vector<std::string> includes;
    std::vector<std::string> cstdeclFunctions;
    std::vector<std::string> globalFunctions;
    std::vector<std::string> body;
};

class SourceParser {
public:
    explicit SourceParser(const std::string& source)
        : source_(source), tokens_(lexCStar(source)) {}

    SourceAst parseAst() {
        SourceAst ast;
        std::vector<std::string> lines;
        std::stringstream stream(source_);
        std::string line;
        while (std::getline(stream, line)) {
            lines.push_back(line);
        }

        for (size_t i = 0; i < lines.size(); ++i) {
            std::string trimmed = trim(lines[i]);
            if (trimmed.empty()) continue;

            std::string transformed = trimmed;
            transformed = std::regex_replace(transformed, std::regex(R"(System\.out\.println)"), "System::out.println");
            transformed = std::regex_replace(transformed, std::regex(R"(^\s*using\s+int\s+main\s*\()"), "int main(");

            if (transformed.find("#include") != std::string::npos) {
                ast.includes.push_back(transformed);
                continue;
            }

            if (transformed.find("__cstdecl") != std::string::npos) {
                FunctionNode node;
                node.kind = "cstdecl";
                CstDeclFunction cst = parseCstDeclSignature(transformed);
                node.returnType = cst.returnType;
                node.name = cst.name;
                node.params = cst.params;

                size_t open = transformed.find('{');
                if (open != std::string::npos) {
                    size_t close = transformed.rfind('}');
                    if (close != std::string::npos && close > open) {
                        std::string inlineBody = transformed.substr(open + 1, close - open - 1);
                        if (!trim(inlineBody).empty()) node.body.push_back(trim(inlineBody));
                        ast.functions.push_back(node);
                        continue;
                    }

                    std::vector<std::string> bodyLines;
                    int depth = 0;
                    bool started = false;
                    for (size_t j = i; j < lines.size(); ++j) {
                        std::string current = trim(lines[j]);
                        if (!started) {
                            if (current.find('{') == std::string::npos) {
                                continue;
                            }
                            started = true;
                        }
                        for (char ch : current) {
                            if (ch == '{') ++depth;
                            if (ch == '}') --depth;
                        }
                        if (depth > 0) {
                            std::string inside = current;
                            if (current.find('{') != std::string::npos) {
                                inside = current.substr(current.find('{') + 1);
                            }
                            if (current.find('}') != std::string::npos) {
                                if (current.find('{') != std::string::npos) {
                                    inside = current.substr(current.find('{') + 1, current.rfind('}') - current.find('{') - 1);
                                } else {
                                    inside = current.substr(0, current.rfind('}'));
                                }
                            }
                            if (!trim(inside).empty()) bodyLines.push_back(trim(inside));
                        }
                        if (depth <= 0 && started) {
                            i = j;
                            break;
                        }
                    }
                    node.body = bodyLines;
                    ast.functions.push_back(node);
                    continue;
                }
            }

            if (transformed.find("int main") != std::string::npos) {
                FunctionNode node;
                node.kind = "main";
                node.returnType = "int";
                node.name = "main";

                size_t open = transformed.find('{');
                if (open != std::string::npos) {
                    size_t close = transformed.rfind('}');
                    if (close != std::string::npos && close > open) {
                        std::string inlineBody = transformed.substr(open + 1, close - open - 1);
                        if (!trim(inlineBody).empty()) node.body.push_back(trim(inlineBody));
                        ast.functions.push_back(node);
                        continue;
                    }
                }

                std::vector<std::string> bodyLines;
                int depth = 0;
                bool started = false;
                for (size_t j = i; j < lines.size(); ++j) {
                    std::string current = trim(lines[j]);
                    if (!started) {
                        if (current.find('{') == std::string::npos) {
                            continue;
                        }
                        started = true;
                        if (current.find('{') != std::string::npos) {
                            std::string beforeOpen = current.substr(0, current.find('{'));
                            if (!beforeOpen.empty() && beforeOpen.find("main") != std::string::npos) {
                                std::string afterOpen = current.substr(current.find('{') + 1);
                                if (current.find('}') != std::string::npos) {
                                    std::string inlineBody = current.substr(current.find('{') + 1, current.rfind('}') - current.find('{') - 1);
                                    if (!trim(inlineBody).empty()) bodyLines.push_back(trim(inlineBody));
                                } else if (!trim(afterOpen).empty()) {
                                    bodyLines.push_back(trim(afterOpen));
                                }
                                if (current.find('}') != std::string::npos) {
                                    i = j;
                                    break;
                                }
                                continue;
                            }
                        }
                    }
                    for (char ch : current) {
                        if (ch == '{') ++depth;
                        if (ch == '}') --depth;
                    }
                    if (current.find('{') != std::string::npos && current.find('}') != std::string::npos) {
                        std::string inside = current.substr(current.find('{') + 1, current.rfind('}') - current.find('{') - 1);
                        if (!trim(inside).empty()) bodyLines.push_back(trim(inside));
                    } else if (current.find('{') != std::string::npos) {
                        std::string inside = current.substr(current.find('{') + 1);
                        if (!trim(inside).empty()) bodyLines.push_back(trim(inside));
                    } else if (current.find('}') != std::string::npos) {
                        std::string beforeBrace = current.substr(0, current.find('}'));
                        if (!trim(beforeBrace).empty()) bodyLines.push_back(trim(beforeBrace));
                    } else {
                        if (!current.empty()) bodyLines.push_back(current);
                    }
                    if (depth <= 0 && started) {
                        i = j;
                        break;
                    }
                }
                node.body = bodyLines;
                ast.functions.push_back(node);
                continue;
            }

            ast.statements.push_back(transformed);
        }

        return ast;
    }

    ParsedSource parse() {
        ParsedSource parsed;

        auto findMatchingBrace = [](const std::string& text, size_t openPos) -> size_t {
            if (openPos >= text.size() || text[openPos] != '{') return std::string::npos;
            int depth = 0;
            bool inString = false;
            char quote = '\0';
            for (size_t i = openPos; i < text.size(); ++i) {
                const char ch = text[i];
                if (inString) {
                    if (ch == quote && (i == 0 || text[i - 1] != '\\')) {
                        inString = false;
                    }
                    continue;
                }
                if (ch == '"' || ch == '\'') {
                    inString = true;
                    quote = ch;
                    continue;
                }
                if (ch == '{') {
                    ++depth;
                } else if (ch == '}') {
                    --depth;
                    if (depth == 0) {
                        return i;
                    }
                }
            }
            return std::string::npos;
        };

        std::string source = source_;
        std::vector<std::pair<size_t, size_t>> skippedRanges;
        size_t pos = 0;
        while (pos < source.size()) {
            const size_t cstPos = source.find("__cstdecl", pos);
            if (cstPos == std::string::npos) break;

            const size_t open = source.find('{', cstPos);
            if (open == std::string::npos) break;
            const size_t close = findMatchingBrace(source, open);
            if (close == std::string::npos) break;

            const std::string signature = trim(source.substr(cstPos, open - cstPos));
            CstDeclFunction cst = parseCstDeclSignature(signature);
            std::stringstream bodyStream(source.substr(open + 1, close - open - 1));
            std::string bodyLine;
            while (std::getline(bodyStream, bodyLine)) {
                std::string cleaned = trim(bodyLine);
                if (!cleaned.empty()) {
                    cst.body.push_back(cleaned);
                }
            }
            parsed.cstdeclFunctions.push_back(renderCstDeclLowering(cst));
            skippedRanges.push_back({cstPos, close + 1});
            pos = close + 1;
        }

        size_t mainPos = 0;
        while (mainPos < source.size()) {
            const size_t found = source.find("int main", mainPos);
            if (found == std::string::npos) break;
            size_t rangeStart = found;
            while (rangeStart > 0 && source[rangeStart - 1] != '\n' && source[rangeStart - 1] != '\r') {
                --rangeStart;
            }
            const size_t open = source.find('{', found);
            if (open == std::string::npos) break;
            const size_t close = findMatchingBrace(source, open);
            if (close == std::string::npos) break;

            std::vector<std::string> bodyLines;
            std::stringstream bodyStream(source.substr(open + 1, close - open - 1));
            std::string bodyLine;
            while (std::getline(bodyStream, bodyLine)) {
                std::string cleaned = trim(bodyLine);
                if (!cleaned.empty()) {
                    if (cleaned == "}" || cleaned == "{" || cleaned == "int main() {" || cleaned == "int main(){" || cleaned == "main() {" || cleaned == "using int main() {" || cleaned == "using int main()") {
                        continue;
                    }
                    cleaned = std::regex_replace(cleaned, std::regex(R"(System\.out\.println)"), "System::out.println");
                    bodyLines.push_back(cleaned);
                }
            }

            for (const auto& line : bodyLines) {
                parsed.body.push_back(line);
            }
            skippedRanges.push_back({rangeStart, close + 1});
            mainPos = close + 1;
        }

        std::sort(skippedRanges.begin(), skippedRanges.end(), [](const auto& a, const auto& b) {
            return a.first > b.first;
        });

        std::string remaining = source_;
        for (const auto& range : skippedRanges) {
            remaining.erase(range.first, range.second - range.first);
        }

        std::stringstream sourceStream(remaining);
        std::string textLine;
        while (std::getline(sourceStream, textLine)) {
            std::string trimmed = trim(textLine);
            if (trimmed.empty()) continue;
            if (trimmed.find("#include") != std::string::npos) continue;
            if (trimmed.find("__cstdecl") != std::string::npos) continue;
            if (trimmed.find("int main") != std::string::npos) continue;
            parsed.globalFunctions.push_back(trimmed);
        }

        return parsed;
    }

private:
    std::string source_;
    std::vector<Token> tokens_;
};

int main(int argc, char* argv[]) {
    std::string filename = "testfile.cstar";
    bool compileFlag = false;
    bool usesArgs = false;
    bool versionFlag = false;
    bool callLinker = false;
    bool linkerVersion = false;
    bool silent = false; // -s silences compiler output
    bool uselib = false;
    std::string libname = "";

    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    #warning "This is an early version of the CStar Compiler. Expect bugs and incomplete features."
    #warning "Recommended to use GCC with -std=gnu++2c for best compatibility with CStar transpiled code."


    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-c") {
            compileFlag = true;
        } else if (endsWith(arg, ".cstar") || endsWith(arg, ".ca")) {
            filename = arg;
        } else if (arg == "--version" || arg == "-v") {
            versionFlag = true;
        } else if (arg == "--lstdcst") {
            callLinker = true;
        } else if (arg == "--lstdcst-v") {
            linkerVersion = true;
        } else if (arg == "-s") {
            silent = true;
        } else if (arg.substr(0, 2) == "-l") {
            uselib = true;
            libname = arg.substr(2);
        }
    }

    // small output helpers that respect -s
    auto printOutln = [&](const std::string &s){
        if (!silent) std::cout << s << std::endl;
    };
    auto printOut = [&](const std::string &s){
        if (!silent) std::cout << s;
    };
    auto printErrln = [&](const std::string &s){
        if (!silent) std::cerr << s << std::endl;
    };

    if (versionFlag) {
        // honor -s: if silent, don't print version info
        if (!silent) {
            std::cout << "\033[1;34mCStar Compiler\033[0m" << std::endl;
            std::cout << "Licensed under the \033[1;31mMIT License\033[0m" << std::endl;
            std::cout << "Version: 2.1.0 Alpha; Language version: " << current_ver << std::endl;
            std::cout << "Copyright (c) October 2026 Hoang Viet. All rights reserved." << std::endl;
        }
        return 0;
    }

    printOutln("\033[1;34mCStar Compiler\033[0m");
    printOutln("Licensed under the \033[1;31mMIT License\033[0m");

    std::ifstream f(filename);
    if (!f.is_open()) {
        printErrln("in \033[1;36mcstcompiler.cpp\033[0m at line 7: error: \033[1mfile not found\033[0m (" + filename + ")");
        return 1;
    }

    // compute cppFilename (safe if filename has no dot)
    auto pos = filename.find_last_of('.');
    std::string base = (pos == std::string::npos) ? filename : filename.substr(0, pos);
    std::string cppFilename = base + ".cpp";

    std::ofstream ofs(cppFilename);
    if (!ofs.is_open()) {
        printErrln("Cannot create output file: " + cppFilename);
        return 1;
    }
    ofs << "// Transpiled from CStar\n";
    ofs << "#include \"ext/stdcstar.h\"\n\n";

    std::vector<std::string> includes;
    std::vector<std::string> body;
    std::vector<std::string> globalFunctions;
    std::vector<std::string> cstdeclFunctions;
    std::string inputText((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    f.close();

    SourceParser parser(inputText);
    ParsedSource initialParse = parser.parse();
    includes.insert(includes.end(), initialParse.includes.begin(), initialParse.includes.end());
    cstdeclFunctions.insert(cstdeclFunctions.end(), initialParse.cstdeclFunctions.begin(), initialParse.cstdeclFunctions.end());
    globalFunctions.insert(globalFunctions.end(), initialParse.globalFunctions.begin(), initialParse.globalFunctions.end());
    body.insert(body.end(), initialParse.body.begin(), initialParse.body.end());

    std::string line;

    // Regex patterns for entry point detection and transformations
    std::regex mainfuncRegex(R"(usingfunc::integerfunc\s+mainfunc\s*\()");
    std::regex usingMainRegex(R"(using\s+int\s+main\s*\()");
    std::regex directMainRegex(R"(^\s*int\s+main\s*\()");
    std::regex functionDefRegex(R"(^\s*(?:[A-Za-z_][A-Za-z0-9_:<>]*\s+)+[A-Za-z_][A-Za-z0-9_]*\s*\([^)]*\)\s*\{)");
    std::regex importRegex(R"(import\s*\(\s*\"([^\"]+)\"\s*,\s*\"([^\"]+)\"\s*\))");
    std::regex sysRegex(R"(System\.out\.println)");
    std::regex strArrayRegex(R"(string\s+args\[\d+\]\s*=\s*\{)");
    std::regex returnfRegex(R"(^\s*returnf\s+)");

    auto transformImportlib = [](const std::string& sourceLine) {
        const std::string marker = "importlib(";
        const auto markerPos = sourceLine.find(marker);
        if (markerPos == std::string::npos) return sourceLine;

        const auto firstQuote = sourceLine.find('"', markerPos + marker.size());
        const auto secondQuote = sourceLine.find('"', firstQuote + 1);
        const auto thirdQuote = sourceLine.find('"', secondQuote + 1);
        const auto fourthQuote = sourceLine.find('"', thirdQuote + 1);
        if (firstQuote == std::string::npos || secondQuote == std::string::npos ||
            thirdQuote == std::string::npos || fourthQuote == std::string::npos) {
            return sourceLine;
        }

        const std::string library = sourceLine.substr(firstQuote + 1, secondQuote - firstQuote - 1);
        const std::string functionName = sourceLine.substr(thirdQuote + 1, fourthQuote - thirdQuote - 1);
        return std::string("auto ") + functionName + " = cst_importlib(\"" + library +
            "\", \"" + functionName + "\");";
    };

    bool inFunctionDefinition = false;
    bool inMainFunction = false;
    bool foundMainDeclaration = false;  // track if we found main() line
    bool inCstDeclFunction = false;
    int braceCount = 0;
    int cstdeclDepth = 0;
    CstDeclFunction currentCstDecl;
    std::vector<std::string> cstdeclBody;

    auto flushCstDeclFunction = [&]() {
        if (!inCstDeclFunction) return;
        cstdeclFunctions.push_back(renderCstDeclLowering(currentCstDecl));
        inCstDeclFunction = false;
        currentCstDecl = CstDeclFunction{};
        cstdeclBody.clear();
        cstdeclDepth = 0;
    };


    // Write includes first (after stdcstar.h)
    for (const auto& inc : includes) {
        ofs << inc << "\n";
    }
    ofs << "\n";
    
    // Write custom CStar calling-convention wrappers
    for (const auto& funcLine : cstdeclFunctions) {
        ofs << funcLine << "\n";
    }
    ofs << "\n";

    // Write global function definitions (helper functions)
    for (const auto& funcLine : globalFunctions) {
        ofs << funcLine << "\n";
    }
    ofs << "\n";

    // Write mainfunc signature
    if (usesArgs) {
        ofs << "int main(int argc, char* argv[]) {\n";
    } else {
        ofs << "int main() {\n";
    }

    for (const auto& b : body) {
        ofs << "    " << b << "\n";
    }

    ofs << "}\n\n";

    ofs.close();

    // Compile to .exe if -c flag is present
    if (compileFlag) {
        std::string includePath = "-I\"D:/CStar/include\"";
        std::string exeFilename = base + ".exe";

        // prefer $CXX if provided, otherwise fall back to g++
        const char* envCxx = std::getenv("CXX");
        std::string compiler = envCxx ? std::string(envCxx) : "g++";

        // use gnu++23 for GNU/Clang toolchains
        std::string stdFlag = "-std=gnu++2c";

        std::string compileCommand = compiler + " " + includePath + " \"" + cppFilename + "\"";
        if (uselib && !libname.empty()) {
            compileCommand += " " + libname + ".cpp";
        }
        compileCommand += " -w " + stdFlag + " -lm -o \"" + exeFilename + "\"";
        printOutln("\033[1;34mCompiling...\033[0m");
        int result = system(compileCommand.c_str());
        std::string executecommand = "\"" + exeFilename + "\"";

        if (result == 0) {
            printOutln("\033[1;32mCompilation successful!\033[0m Output: " + exeFilename);
            if (!silent) system(executecommand.c_str());
        } else {
            printErrln("\033[1;31mCompilation failed.\033[0m");
            return 1;
        }
    }

    // If --lstdcst was specified, call linker *after* compilation is done
    if (callLinker) {
        std::string callLinkerCommand = "linker.bat \"" + cppFilename + "\"";
        printOutln("\033[1;34mInvoking linker...\033[0m");
        int linkResult = system(callLinkerCommand.c_str());
        if (linkResult != 0) {
            printErrln("\033[1;31mLinker failed.\033[0m");
            return 1;
        } else {
            printOutln("\033[1;32mLinking successful!\033[0m");
        }
    }

    if (linkerVersion) {
        std::string callLinkerVersionCommand = "linker.bat -v";
        if (!silent) system(callLinkerVersionCommand.c_str());
    }

    return 0;
}

/*
As Bjarne said, C++ makes us blow off our foots. But CStar makes you put your leg into the CERN LHC.

- Hoang Viet, November 2025
*/

// recommended to use g++ or clang++ with -std=gnu++2c for best compatibility with CStar transpiled code.