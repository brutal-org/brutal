#pragma once

#define Mat3x2(T) \
    struct        \
    {             \
        T m[6];   \
    }

typedef Mat3x2(int) Mat3x2i;
typedef Mat3x2(float) Mat3x2f;
