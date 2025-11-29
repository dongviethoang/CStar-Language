#pragma once

#ifndef CSTLIB26_STDCSTAR_H
#define CSTLIB26_STDCSTAR_H 1

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <cstring>
#include <csignal>
#include <set>
#include <cstdlib>
#include <type_traits>
#include <cstdint>
#include <cstdio>
#include "../cstar.h"
#include "../../keywords.h"

#if !defined(_MSC_VER)
    #ifndef __cdecl
        #define __cdecl
    #endif
#endif

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN  // Exclude rarely-used Windows stuff
    #define NOMINMAX              // Prevent Windows min/max macros
    #include <windows.h>
    // previously: #undef byte
#endif

#define EXIT_S 0
#define EXIT_F 1

typedef const char* POSIXSTR;

// Use a small inline C array of C-strings for identifiers to avoid static init order issues
inline constexpr const char* CSTAR_IDENTIFIERS[] = { "#s", "#d", "#f", "#c" };

class UNIX {
public:
    // Print string variant. Returns length printed (or -1 on error).
    static int __cdecl posixprintf(bool endline, const char* id, const std::string& message) {
        if (std::strcmp(id, CSTAR_IDENTIFIERS[0]) != 0) return -1;
        return endline ? std::printf("%s\n", message.c_str()) : std::printf("%s", message.c_str());
    }

    // Print int variant.
    static int __cdecl posixprintf(bool endline, const char* id, int message) {
        if (std::strcmp(id, CSTAR_IDENTIFIERS[1]) != 0) return -1;
        return endline ? std::printf("%d\n", message) : std::printf("%d", message);
    }

    // Print floating-point variant (double to cover float/double).
    static int __cdecl posixprintf(bool endline, const char* id, double message) {
        if (std::strcmp(id, CSTAR_IDENTIFIERS[2]) != 0) return -1;
        return endline ? std::printf("%f\n", message) : std::printf("%f", message);
    }

    static int __cdecl posixprintf(bool endline, const char* id, char message) {
        if (std::strcmp(id, CSTAR_IDENTIFIERS[3]) != 0) return -1;
        return endline ? std::printf("%c\n", message) : std::printf("%c", message);
    }

    // --- scanf variants: accept non-const references and use correct formats ---
    static int __cdecl posixscanf(const char* id, std::string &out_value) {
        if (std::strcmp(id, CSTAR_IDENTIFIERS[0]) != 0) return -1;
        char buffer[1024];
        int result = std::scanf("%1023s", buffer);
        if (result == 1) out_value = buffer;
        return result;
    }

    static int __cdecl posixscanf(const char* id, int &out_value) {
        if (std::strcmp(id, CSTAR_IDENTIFIERS[1]) != 0) return -1;
        return std::scanf("%d", &out_value);
    }

    static int __cdecl posixscanf(const char* id, double &out_value) {
        if (std::strcmp(id, CSTAR_IDENTIFIERS[2]) != 0) return -1;
        return std::scanf("%lf", &out_value); /* use %lf for double */
    }

    static int __cdecl posixscanf(const char* id, char &out_value) {
        if (std::strcmp(id, CSTAR_IDENTIFIERS[3]) != 0) return -1;
        return std::scanf(" %c", &out_value); /* skip whitespace */
    }

    // Generic fallback: convert arithmetic types to double/int and strings to std::string
    template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    static inline int posixprintf(bool endline, const char* id, T message) {
        if constexpr (std::is_integral_v<T>) {
            return posixprintf(endline, id, static_cast<int>(message));
        } else {
            return posixprintf(endline, id, static_cast<double>(message));
        }
    }

    template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    static inline int posixscanf(const char* id, T &out_value) {
        if constexpr (std::is_integral_v<T>) {
            int tmp = 0;
            int r = posixscanf(id, tmp);
            if (r > 0) out_value = static_cast<T>(tmp);
            return r;
        } else {
            double tmp = 0.0;
            int r = posixscanf(id, tmp);
            if (r > 0) out_value = static_cast<T>(tmp);
            return r;
        }
    }
};

class CONSOLE {
public:
    template<typename T>
    static inline void WriteLine(const T& message) {
        std::cout << message << std::endl;
    }
    static inline void SBeep() {
        #ifdef _WIN32
            Beep(523, 500);
        #else
            std::cout << '\a' << std::flush;
        #endif 
    }
    template<typename T>
    static inline void Error(const T& message) {
        std::cerr << message << std::endl;
    }
    template<typename T>
    static inline void ReadLine(T& input) {
        if constexpr (std::is_same_v<T, std::string>) {
            std::getline(std::cin, input);
        } else {
            std::cin >> input;
        }
    }
};

// inline globals to avoid ODR/linker problems
inline CONSOLE Console;
inline UNIX posix_util;

// small compatibility namespace alias (avoid leaking platform-conflicting names)
namespace stdcst {
    using cstarstr = char*;
}

#endif // CSTLIB26_STDCSTAR_H
