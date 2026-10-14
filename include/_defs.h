#ifndef _CST_DEFS_H
#define _CST_DEFS_H

#include <fstream>
#include <cstdint>

#if defined(_MSC_VER)
    #error "This project does not support Microsoft Visual C++ compiler."
#elif defined(__GNUC__) || defined(__GNUG__)
    #if __cplusplus < 202302L
        #error "This project requires C++23 or later."
    #endif
#elif defined(__clang__)
    #if __cplusplus < 202302L
        #error "This project requires C++23 or later."
    #endif
#endif

struct cstdecl {
    #warning "This feature is only supported from CStar26 Debug 3 onwards. It may not be available in earlier versions. And it is buggy."
    std::ofstream cstlog;
    cstdecl()
        : cstlog("switch.log", std::ios::out | std::ios::app)
    {
        cstlog << "compile stack c style" << std::endl;
        cstlog << "pascal style disabled" << std::endl;
        cstlog.close();
    }
};

extern cstdecl __cstdecl;

namespace asm_util {
    typedef int64_t rax;
    typedef int64_t rbx;
    typedef int64_t rcx;
    typedef int64_t rdx;
    // x86rax
    typedef int32_t eax;
    typedef int32_t ebx;
    typedef int32_t ecx;
    typedef int32_t edx;
    // i386
    typedef int16_t ax;
    typedef int16_t bx;
    typedef int16_t cx;
    typedef int16_t dx;
    // 8-bit
    typedef int8_t al;
    typedef int8_t bl;
    typedef int8_t cl;
    typedef int8_t dl;
    typedef uint8_t ah;
    typedef uint8_t bh;
    typedef uint8_t ch;
    typedef uint8_t dh;
}

#endif
