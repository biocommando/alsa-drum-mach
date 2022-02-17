#ifndef DRUM_MACH_H
#define DRUM_MACH_H

// 128 midi notes so no point in defining more than that
#define MAX_NUM_SLOTS 128

#define PARAM_ID_SPEED 1
#define PARAM_ID_VOL 2

#define NUM_PARAMS 2

void init_drum_mach(int sample_rate);
void deinit_drum_mach();
void drum_mach_trigger(int idx);
void drum_mach_set_param(int idx, int param, float param_val);
void drum_mach_process_audio(short *data, int count_samples, int is_stereo);

struct midi_cc_setup
{
    int param_id;
    int cc;
};

struct midi_setup
{
    int note_offset;
    int port;
    int num_slots;
    struct midi_cc_setup cc_setup[MAX_NUM_SLOTS][NUM_PARAMS];
};

struct midi_setup get_midi_setup();

#endif