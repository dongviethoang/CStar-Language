/*
    * Copyright (c) 2026 Hoang Viet. All rights reserved.
    * Licensed under the MIT License. See LICENSE file in the project root for full license information
    * Inspired by Borland. Although the file was named _defs.h, it is not a copy at all.
    * This file is part of the CStar project and is intended to provide definitions and utilities for the CStar programming language.
    * And definitely no C++98. ANCIENT.
    * 
    * __cstdecl is bugging out in here, plus it is already defined in the compiler, so I will change it to a comment later.
    * I'm crazy enough to add Assembly registers, and I don't know if they have those data types.
    * Great, I DIDN'T KNOW ABOUT x87.
    * what do you mean 80-bit, WHAT???
*/

#ifndef _CST_DEFS_H
#define _CST_DEFS_H

#include <fstream>
#include <cstdint>
#include <cfloat>
#include <stdfloat>

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
    // floats (compiler padded to 128-bit because there's like no 80-bit float type in C++)
    typedef std::float128_t st0;
    typedef std::float128_t st1;
    typedef std::float128_t st2;
    typedef std::float128_t st3;
    typedef std::float128_t st4;
    typedef std::float128_t st5;
    typedef std::float128_t st6;
    typedef std::float128_t st7;
}

#endif
