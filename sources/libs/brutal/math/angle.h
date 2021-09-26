#pragma once 

#include <math.h>

typedef float Degree;
typedef float Radian;

#define deg2rad(x) \
    ((x)*180/M_PI)

#define rad2deg(x) \
    ((x)*M_PI/180)
