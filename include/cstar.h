#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <_defs.h>

cstdecl __cstdecl;

// Forward declarations
class Out;

class System {
public:
    static Out out;
    
    template<typename T>
    static void println(const T& message) {
        std::cout << message << std::endl;
    }
};

class Out {
public:
    template<typename T>
    static void println(const T& message) {
        System::println(message);
    }
    template<typename T>
    static void print(const T& message) {
        std::cout << message;
    }
};

// Initialize static member (inline to avoid multiple definition errors)
inline Out System::out;

namespace usingfunc {
    using integerfunc = int;
}

namespace cstar25 {
    template<typename T>
    void pinput(T& input, const std::string& prompt = "") {
        std::cout << prompt;
        std::cin >> input;
    }
}

namespace cpp20 {
    template<typename T>
    inline void println(const T& msg) {
        std::cout << msg << std::endl;
    }
}

class Delay {
    public:
    static inline void ms(int milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
};

inline Delay delay;

using integer = int;
using str = std::string;

// rtrn keyword (shorthand for return)
#define rtrn return

// Optional helper to wrap argv
inline std::vector<std::string> getArgs(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return args;
}