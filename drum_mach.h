#ifndef DRUM_MACH_H
#define DRUM_MACH_H

#define PARAM_ID_SPEED 1
#define PARAM_ID_VOL 2

void init_drum_mach(int sample_rate);
void deinit_drum_mach();
void drum_mach_trigger(int idx);
void drum_mach_set_param(int idx, int param, float param_val);
void drum_mach_process_audio(short *data, int count_samples, int is_stereo);


#endif