#pragma once

#define Mat3x2(T) \
    struct        \
    {             \
        T m[6];   \
    }

typedef Mat3x2(int) Mat3x2i;
typedef Mat3x2(float) Mat3x2f;

#define mat3x2_indentity(T) \
    (T)                     \
    {                       \
        .m = { 1.0f,        \
               0.0f,        \
               0.0f,        \
               1.0f,        \
               0.0f,        \
               0.0f,        \
        }                   \
    }
