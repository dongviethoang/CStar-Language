/*
This is the CStar Compiler.
Yes, it's in C++, for sure.
(and definitely not Rust)
Do not attempt to modify the source. (except me)
How complex is this compiler? Uh... quite complex.
I don't even know where is the parser, if it has one.
No wait, it doesn't have one, it's like an interpreter without a lexer... plus transpiler.
How do I build a lexer, and parser???????

Copyright (c) November 2025 Hoang Viet. All rights reserved.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <cstdlib>   // for system/getenv
#include "keywords.h"

bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string current_ver = "CStar26 Debug 3";

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
            std::cout << "Version: 1.5.9; Language version: " << current_ver << std::endl;
            std::cout << "Copyright (c) August 2025 Hoang Viet. All rights reserved." << std::endl;
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
    std::string line;

    // Regex patterns for entry point detection and transformations
    std::regex mainfuncRegex(R"(usingfunc::integerfunc\s+mainfunc\s*\()");
    std::regex usingMainRegex(R"(using\s+int\s+main\s*\()");
    std::regex importRegex(R"(import\s*\(\s*\"([^\"]+)\"\s*,\s*\"([^\"]+)\"\s*\))");
    std::regex sysRegex(R"(System\.out\.println)");
    std::regex strArrayRegex(R"(string\s+args\[\d+\]\s*=\s*\{)");
    std::regex returnfRegex(R"(^\s*returnf\s+)");

    bool inFunctionDefinition = false;
    bool inMainFunction = false;
    bool foundMainDeclaration = false;  // track if we found main() line
    int braceCount = 0;

    while (std::getline(f, line)) {
        bool keywordFound = false;

        for (const auto& keyword : keywords) {
            if (line.find(keyword) != std::string::npos) {
                printOutln("Keyword found: " + keyword + " in line: " + line);
                keywordFound = true;
                break;
            }
        }

        // Detect argument usage
        if (line.find("argc") != std::string::npos || 
            line.find("argv") != std::string::npos || 
            line.find("args") != std::string::npos) {
            usesArgs = true;
        }

        // Handle import() function
        std::smatch importMatch;
        if (std::regex_search(line, importMatch, importRegex)) {
            std::string headerName = importMatch[1].str();
            std::string headerType = importMatch[2].str();
            
            if (headerType == "system") {
                includes.push_back("#include <" + headerName + ">");
                printOutln("Import found: system header <" + headerName + ">");
            } else if (headerType == "local") {
                includes.push_back("#include \"" + headerName + "\"");
                printOutln("Import found: local header \"" + headerName + "\"");
            } else {
                printErrln("\033[1;33mWarning:\033[0m Unknown import type '" + headerType + "'. Use 'system' or 'local'.");
            }
            
            continue;
        }

        // Handle traditional includes
        if (line.find("#include") != std::string::npos) {
            if (line.find("ext/stdcstar.h") == std::string::npos) {
                includes.push_back(line);
            }
            continue;
        }

        // Detect main function declaration
        if (std::regex_search(line, mainfuncRegex) || std::regex_search(line, usingMainRegex)) {
            foundMainDeclaration = true;
            
            // Count braces on this line
            for (char c : line) {
                if (c == '{') {
                    braceCount++;
                    inMainFunction = true;  // Found opening brace
                }
                if (c == '}') braceCount--;
            }
            
            // If we found opening brace on same line, we're in main body now
            // If not, we'll catch it on next line
            continue;
        }
        
        // If we found main declaration but haven't entered body yet
        if (foundMainDeclaration && !inMainFunction) {
            // Look for opening brace
            for (char c : line) {
                if (c == '{') {
                    braceCount++;
                    inMainFunction = true;
                    break;  // Found it, now we're in main body
                }
            }
            
            // If this line only has the brace, skip it
            if (inMainFunction && line.find_first_not_of(" \t{") == std::string::npos) {
                continue;
            }
        }
        
        // Process main function body
        if (inMainFunction) {
            // Count braces
            for (char c : line) {
                if (c == '{') braceCount++;
                if (c == '}') braceCount--;
            }
            
            // If we hit the closing brace of main, stop
            if (braceCount == 0 && line.find('}') != std::string::npos) {
                inMainFunction = false;
                foundMainDeclaration = false;
                continue;
            }
            
            // Apply transformations
            line = std::regex_replace(line, sysRegex, "System::out.println");
            line = std::regex_replace(line, strArrayRegex, "std::string args[] = {");
            
            body.push_back(line);
            continue;
        }
        
        // Handle returnf function definitions
        if (std::regex_search(line, returnfRegex) && !inFunctionDefinition) {
            inFunctionDefinition = true;
            braceCount = 0;
            line = std::regex_replace(line, returnfRegex, "");
        }

        if (inFunctionDefinition) {
            globalFunctions.push_back(line);
            
            // Count braces
            for (char c : line) {
                if (c == '{') braceCount++;
                if (c == '}') braceCount--;
            }
            
            if (braceCount == 0 && line.find('}') != std::string::npos) {
                inFunctionDefinition = false;
            }
            continue;
        }

        // Fix System.out.println → System::out.println
        line = std::regex_replace(line, sysRegex, "System::out.println");

        // Fix string args declaration
        line = std::regex_replace(line, strArrayRegex, "std::string args[] = {");

        body.push_back(line);
    }

    f.close();

    // Write includes first (after stdcstar.h)
    for (const auto& inc : includes) {
        ofs << inc << "\n";
    }
    ofs << "\n";
    
    // Write global function definitions (helper functions)
    for (const auto& funcLine : globalFunctions) {
        ofs << funcLine << "\n";
    }
    ofs << "\n";

    // Write mainfunc signature
    if (usesArgs) {
        ofs << "usingfunc::integerfunc mainfunc(int argc, char* argv[]) {\n";
    } else {
        ofs << "usingfunc::integerfunc mainfunc() {\n";
    }

    for (const auto& b : body) {
        ofs << "    " << b << "\n";
    }

    ofs << "}\n\n";

    // Write main wrapper
    if (usesArgs) {
        ofs << "int main(int argc, char* argv[]) {\n    return mainfunc(argc, argv);\n}\n";
    } else {
        ofs << "int main() {\n    return mainfunc();\n}\n";
    }

    ofs.close();

    // Compile to .exe if -c flag is present
    if (compileFlag) {
        std::string includePath = "-I\"D:/CStar/include\"";
        std::string exeFilename = base + ".exe";

        // prefer $CXX if provided, otherwise fall back to g++
        const char* envCxx = std::getenv("CXX");
        std::string compiler = envCxx ? std::string(envCxx) : "g++";

        // use gnu++23 for GNU/Clang toolchains
        std::string stdFlag = "-std=gnu++23";

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