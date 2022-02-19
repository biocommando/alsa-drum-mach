#ifndef DRUM_MACH_FLT_H
#define DRUM_MACH_FLT_H

void drum_mach_filter_init(int sample_rate);
double drum_mach_filter_calculate(double input);
void drum_mach_filter_set_cutoff(double c);
void drum_mach_filter_set_resonance(double r);

#endif
