#include <math.h>

#include "easing.h"

double m_easing_linear(double p)
{
    return p;
}

double m_easing_quadratic_in(double p)
{
    return p * p;
}

double m_easing_quadratic_out(double p)
{
    return -(p * (p - 2));
}

double m_easing_quadratic_in_out(double p)
{
    if (p < 0.5)
    {
        return 2 * p * p;
    }
    else
    {
        return (-2 * p * p) + (4 * p) - 1;
    }
}

double m_easing_cubic_in(double p)
{
    return p * p * p;
}

double m_easing_cubic_out(double p)
{
    double f = (p - 1);
    return f * f * f + 1;
}

double m_easing_cubic_in_out(double p)
{
    if (p < 0.5)
    {
        return 4 * p * p * p;
    }
    else
    {
        double f = ((2 * p) - 2);
        return 0.5 * f * f * f + 1;
    }
}

double m_easing_quartic_in(double p)
{
    return p * p * p * p;
}

double m_easing_quartic_out(double p)
{
    double f = (p - 1);
    return f * f * f * (1 - p) + 1;
}

double m_easing_quartic_in_out(double p)
{
    if (p < 0.5)
    {
        return 8 * p * p * p * p;
    }
    else
    {
        double f = (p - 1);
        return -8 * f * f * f * f + 1;
    }
}

double m_easing_quintic_in(double p)
{
    return p * p * p * p * p;
}

double m_easing_quintic_out(double p)
{
    double f = (p - 1);
    return f * f * f * f * f + 1;
}

double m_easing_quintic_in_out(double p)
{
    if (p < 0.5)
    {
        return 16 * p * p * p * p * p;
    }
    else
    {
        double f = ((2 * p) - 2);
        return 0.5 * f * f * f * f * f + 1;
    }
}

double m_easing_sine_in(double p)
{
    return sin((p - 1) * M_PI_2) + 1;
}

double m_easing_sine_out(double p)
{
    return sin(p * M_PI_2);
}

double m_easing_sine_in_out(double p)
{
    return 0.5 * (1 - cos(p * M_PI));
}

double m_easing_circular_in(double p)
{
    return 1 - sqrt(1 - (p * p));
}

double m_easing_circular_out(double p)
{
    return sqrt((2 - p) * p);
}

double m_easing_circular_in_out(double p)
{
    if (p < 0.5)
    {
        return 0.5 * (1 - sqrt(1 - 4 * (p * p)));
    }
    else
    {
        return 0.5 * (sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
    }
}

double m_easing_exponential_in(double p)
{
    return (p == 0.0) ? p : pow(2, 10 * (p - 1));
}

double m_easing_exponential_out(double p)
{
    return (p == 1.0) ? p : 1 - pow(2, -10 * p);
}

double m_easing_exponential_in_out(double p)
{
    if (p == 0.0 || p == 1.0)
    {
        return p;
    }

    if (p < 0.5)
    {
        return 0.5 * pow(2, (20 * p) - 10);
    }
    else
    {
        return -0.5 * pow(2, (-20 * p) + 10) + 1;
    }
}

double m_easing_elastic_in(double p)
{
    return sin(13 * M_PI_2 * p) * pow(2, 10 * (p - 1));
}

double m_easing_elastic_out(double p)
{
    return sin(-13 * M_PI_2 * (p + 1)) * pow(2, -10 * p) + 1;
}

double m_easing_elastic_in_out(double p)
{
    if (p < 0.5)
    {
        return 0.5 * sin(13 * M_PI_2 * (2 * p)) * pow(2, 10 * ((2 * p) - 1));
    }
    else
    {
        return 0.5 * (sin(-13 * M_PI_2 * ((2 * p - 1) + 1)) * pow(2, -10 * (2 * p - 1)) + 2);
    }
}

double m_easing_back_in(double p)
{
    return p * p * p - p * sin(p * M_PI);
}

double m_easing_back_out(double p)
{
    double f = (1 - p);
    return 1 - (f * f * f - f * sin(f * M_PI));
}

double m_easing_back_in_out(double p)
{
    if (p < 0.5)
    {
        double f = 2 * p;
        return 0.5 * (f * f * f - f * sin(f * M_PI));
    }
    else
    {
        double f = (1 - (2 * p - 1));
        return 0.5 * (1 - (f * f * f - f * sin(f * M_PI))) + 0.5;
    }
}

double m_easing_bounce_out(double p)
{
    if (p < 4 / 11.0)
    {
        return (121 * p * p) / 16.0;
    }
    else if (p < 8 / 11.0)
    {
        return (363 / 40.0 * p * p) - (99 / 10.0 * p) + 17 / 5.0;
    }
    else if (p < 9 / 10.0)
    {
        return (4356 / 361.0 * p * p) - (35442 / 1805.0 * p) + 16061 / 1805.0;
    }
    else
    {
        return (54 / 5.0 * p * p) - (513 / 25.0 * p) + 268 / 25.0;
    }
}

double m_easing_bounce_in(double p)
{
    return 1 - m_easing_bounce_out(1 - p);
}

double m_easing_bounce_in_out(double p)
{
    if (p < 0.5)
    {
        return 0.5 * m_easing_bounce_in(p * 2);
    }
    else
    {
        return 0.5 * m_easing_bounce_out(p * 2 - 1) + 0.5;
    }
}
