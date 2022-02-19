#ifndef DRUM_MACH_H
#define DRUM_MACH_H

// 128 midi notes so no point in defining more than that
#define MAX_NUM_SLOTS 128

#define PARAM_ID_SPEED 1
#define PARAM_ID_VOL 2

#define GLOBAL_PARAM_ID_VOL 2
#define GLOBAL_PARAM_ID_USE_FLT 10
#define GLOBAL_PARAM_ID_FLT_CUT 11
#define GLOBAL_PARAM_ID_FLT_RES 12
#define GLOBAL_PARAM_ID_FLT_PRE_OVRDRV 13

#define NUM_PARAMS 2
#define NUM_GLOBAL_PARAMS 5

#define NUM_CC_MAPPINGS (NUM_PARAMS * MAX_NUM_SLOTS + NUM_GLOBAL_PARAMS)

void init_drum_mach(int sample_rate, int kit_number);
void deinit_drum_mach();
void drum_mach_trigger(int idx);
void drum_mach_set_param(int idx, int param, float param_val);
void drum_mach_process_audio(short *data, int count_samples, int is_stereo);

struct midi_cc_setup
{
    int slot;
    int param_id;
    int cc;
    float min;
    float max;
};

struct midi_setup
{
    int note_offset;
    int port;
    int num_slots;
    struct midi_cc_setup cc_setup[NUM_CC_MAPPINGS];
};

struct midi_setup get_midi_setup();

#endif
