#pragma once

#define math_min(a, b) ((a) < (b) ? (a) : (b))
#define math_max(a, b) ((a) > (b) ? (a) : (b))
#define math_clamp(x, a, b) math_min(b, math_max(a, x))
