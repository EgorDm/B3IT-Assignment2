//
// Created by egordm on 19-3-18.
//
#include "macros.h"
#include <iostream>


void __MM_Assert(const char *expr_str, bool expr, const char *file, int line, const char *msg) {
    if (!expr)
    {
        std::cerr << "Assert failed:\t" << msg << "\n"
                  << "Expected:\t" << expr_str << "\n"
                  << "Source:\t\t" << file << ", line " << line << "\n";
        abort();
    }
}