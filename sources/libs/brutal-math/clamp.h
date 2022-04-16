#pragma once

#define m_min(a, b) ((a) < (b) ? (a) : (b))
#define m_max(a, b) ((a) > (b) ? (a) : (b))
#define m_clamp(x, a, b) m_min(b, m_max(a, x))
