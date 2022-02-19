#include "filter.h"
double p0;
double p1;
double p2;
double p3;
double p32;
double p33;
double p34;
double cutoff, coCalc;
double resonance;
double sampleRate;
const double to_1 = 1.0 / 0x8000;
const double from_1 = 0x8000; 
double pre_overdrive = 1;

inline static double fast_tanh(double x)
{
    const double x2 = x * x;
    return x * (27.0 + x2) / (27.0 + 9.0 * x2);
}

inline static void calculateCutoff()
{
    coCalc = cutoff;
    coCalc = coCalc * 44100 / sampleRate;// 6.28318530717 * 1000 / sampleRate;
    coCalc = coCalc > 1 ? 1 : (coCalc < 0 ? 0 : coCalc);
}

void drum_mach_filter_init(int sample_rate)
{
    sampleRate = sample_rate;
    p0 = p1 = p2 = p3 = p32 = p33 = p34 = 0.0;
    resonance = 0;
    drum_mach_filter_set_cutoff(1);
}

double drum_mach_filter_calculate(double x)
{
    x *= to_1;
    x = fast_tanh(x * pre_overdrive);
    const double k = 4 * resonance;
    // Coefficients optimized using differential evolution
    // to make feedback gain 4.0 correspond closely to the
    // border of instability, for all values of omega.
    const double out = p3 * 0.360891 + p32 * 0.417290 + p33 * 0.177896 + p34 * 0.0439725;

    p34 = p33;
    p33 = p32;
    p32 = p3;

    p0 += (fast_tanh(x - k * out) - fast_tanh(p0)) * coCalc;
    p1 += (fast_tanh(p0) - fast_tanh(p1)) * coCalc;
    p2 += (fast_tanh(p1) - fast_tanh(p2)) * coCalc;
    p3 += (fast_tanh(p2) - fast_tanh(p3)) * coCalc;

    return out * from_1;
}

void drum_mach_filter_set_cutoff(double c)
{
    cutoff = c;
    calculateCutoff();
}

void drum_mach_filter_set_resonance(double r)
{
    resonance = r;
}

void drum_mach_filter_set_pre_overdrive(double od)
{
    pre_overdrive = 1 + od * 10;
}


