//
// Created by egordm on 19-3-18.
//

#ifndef B3IT_ASSIGNMENT2_MACROS_H
#define B3IT_ASSIGNMENT2_MACROS_H

#include "defines.h"

#if DEBUG
#   define M_Assert(Expr, Msg) __MM_Assert(#Expr, Expr, __FILE__, __LINE__, Msg)
#else
#   define M_Assert(Expr, Msg) ;
#endif

void __MM_Assert(const char* expr_str, bool expr, const char* file, int line, const char* msg);

#define TOPIC(topic) (MQTT_ROOT_TOPIC + (topic)).c_str()

#endif //B3IT_ASSIGNMENT2_MACROS_H
