#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "drum_mach_conf_preproc.h"
#include "drum_mach.h"
#include "filter.h"
#include "log.h"

#define LOG_ID "DRUM_MACH"

struct sample_data
{
    short *buf;
    int size;
    float pos;
    float speed;
    float vol;
};

struct sample_data sample_data_arr[MAX_NUM_SLOTS];

struct midi_cc_setup cc_mappings[NUM_CC_MAPPINGS];

void init_cc_mappings()
{
    for (int i = 0; i < NUM_CC_MAPPINGS; i++)
        cc_mappings[i].cc = -1;
}

struct midi_cc_setup *get_next_cc()
{
    for (int i = 0; i < NUM_CC_MAPPINGS; i++)
    {
        if (cc_mappings[i].cc == -1)
            return &cc_mappings[i];
    }
    return NULL;
}

int init_done = 0;
int num_samples = MAX_NUM_SLOTS;
int midi_note_offset = 0;
int midi_port = -1;
float speed_mult = 1;

/* global parameters */
float global_volume = 1;
float global_use_filter = 0;

void init_drum_mach(int sample_rate)
{
    if (init_done)
        return;
    drum_mach_filter_init(sample_rate);
    init_cc_mappings();

    FILE *fbin, *fconfig;

    speed_mult = 44100.0f / sample_rate;

    fconfig = fopen("path_config.txt", "r");
    if (!fconfig)
    {
        log_error("Could not open path_config.txt\n");
        return;
    }
    char sample_data_path[256];
    fscanf(fconfig, "%s", sample_data_path);
    fclose(fconfig);

    char config_path[280];
    sprintf(config_path, "%ssample_data_config.txt", sample_data_path);
    log_info("Load config from '%s'\n", config_path);
    fconfig = fopen(config_path, "r");
    if (!fconfig)
    {
        log_error("Could not open file!\n");
        return;
    }

    memset(sample_data_arr, 0, sizeof(sample_data_arr));
    for (int i = 0; i < MAX_NUM_SLOTS; i++)
    {
        sample_data_arr[i].speed = speed_mult;
        sample_data_arr[i].vol = 0.5;
    }
    
    int end_config_found = 0;

    while (!feof(fconfig) && !end_config_found)
    {
        char readbuf[256], preproc_buf[1024] = "";
        fgets(readbuf, 256, fconfig);
        preprocess_line(preproc_buf, 1024, readbuf, sample_data_path);
        
        char cmd = preproc_buf[0];
        char *cmd_params = preproc_buf + 1;
        
        switch (cmd)
        {
            case '\0':
            case '\n':
            case '.':
            break;
            case 'L': // Load sample
            {
                int slot = 0;
                int sz = 0;
                char sample_name[256];
                
                sscanf(cmd_params, "%d %s %d", &slot, sample_name, &sz);
                
                char fname[1024];
                sprintf(fname, "%s%s", sample_data_path, sample_name);

                log_info("Load sample '%s' (length %d) to slot %d\n", fname, sz, slot);

                fbin = fopen(fname, "rb");

                if (sz && slot >= 0 && slot < MAX_NUM_SLOTS)
                {
                    sample_data_arr[slot].buf = malloc(sz * sizeof(short));
                    fread(sample_data_arr[slot].buf, sizeof(short), sz, fbin);
                    sample_data_arr[slot].size = sz;
                    sample_data_arr[slot].pos = sz;
                }
                fclose(fbin);
            }
            break;
            case '#': // # of slots in use
            sscanf(cmd_params, "%d", &num_samples);
            log_info("Slots in use %d\n", num_samples);
            break;
            case 'M': // MIDI config
            sscanf(cmd_params, "%d %d", &midi_note_offset, &midi_port);
            log_info("Midi note offset %d, port %d\n", midi_note_offset, midi_port);            
            break;
            case 'C': // CC mapping
            {
                int slot = 0;
                int cc = -1;
                float min = 0;
                float max = 1;
                int param_id;
                sscanf(cmd_params, "%d %d %d %f %f", &slot, &param_id, &cc, &min, &max);
                struct midi_cc_setup *m = get_next_cc();
                if (m)
                {
                    m->slot = slot;
                    m->param_id = param_id;
                    m->cc = cc;
                    m->min = min;
                    m->max = max;
                    log_info("Add CC mapping: slot %d param %d cc %d %f - %f\n", slot, param_id, cc, min, max);
                }
            }
            break;
            case 'P': // Parameter values
            {
                int slot = 0;
                int param_id = PARAM_ID_SPEED;
                float value = 1;
                sscanf(cmd_params, "%d %d %f", &slot, &param_id, &value);
                if (slot >= -1 && slot < MAX_NUM_SLOTS)
                {
                    log_info("Set param: slot %d param %d = %f\n", slot, param_id, value);
                    drum_mach_set_param(slot, param_id, value);
                }
            }
            break;
            case 'E':
            end_config_found = 1;
            break;
            default:
                log_warn("Unknown command '%c'\n", cmd);
            break;
        }
    }
    fclose(fconfig);
    
    if (!end_config_found)
    {
        log_error("config file end command (E) not found\n");
        return;
    }
    
    if (num_samples <= 0 || num_samples > MAX_NUM_SLOTS)
    {
        log_error("Erroneous number of samples (%d)\n", num_samples);
        return;
    }
    for (int i = 0; i < num_samples; i++)
    {
        if (!sample_data_arr[i].buf)
        {
            log_error("no sample loaded in slot %d\n", i);
            return;
        }
    }
    
    log_info("Drum machine init successful\n");
        
    init_done = 1;
}

void deinit_drum_mach()
{
    if (!init_done)
        return;
    log_info("Free drum samples\n");
    for (int i = 0; i < num_samples; i++)
        free(sample_data_arr[i].buf);
    init_done = 0;
}

void drum_mach_trigger(int idx)
{
    if (idx >= 0 && idx < num_samples)
    {
        sample_data_arr[idx].pos = 0;
    }
}

void drum_mach_set_param(int idx, int param, float param_val)
{
    if (idx == -1)
    {
        if (param == GLOBAL_PARAM_ID_VOL)
            global_volume = param_val;
        else if (param == GLOBAL_PARAM_ID_USE_FLT)
            global_use_filter = param_val;
        else if (param == GLOBAL_PARAM_ID_FLT_CUT)
            drum_mach_filter_set_cutoff(param_val);
        else if (param == GLOBAL_PARAM_ID_FLT_RES)
            drum_mach_filter_set_resonance(param_val);
        else if (param == GLOBAL_PARAM_ID_FLT_PRE_OVRDRV)
            drum_mach_filter_set_pre_overdrive(param_val);
        return;
    }
    if (idx >= 0 && idx < num_samples)
    {
        if (param == PARAM_ID_SPEED)
            sample_data_arr[idx].speed = param_val * speed_mult;
        else if (param == PARAM_ID_VOL)
            sample_data_arr[idx].vol = param_val;
    }
}

void drum_mach_process_audio(short *data, int count_samples, int is_stereo)
{
    if (!init_done)
        return;
    for (int i = 0; i < count_samples; i++)
    {
        double sample_val = 0;
        for (int j = 0; j < num_samples; j++)
        {
            struct sample_data *d = &sample_data_arr[j];
            if (d->pos < d->size)
            {
                sample_val += d->buf[(int)d->pos] * d->vol;
                d->pos += d->speed;
            }
        }
        sample_val *= global_volume;
        if (global_use_filter > 0.5)
        {
            sample_val = drum_mach_filter_calculate(sample_val);
        }
        sample_val = sample_val < -32768 ? -32768 : (sample_val > 32767 ? 32767 : sample_val);
        const short s_sample_val = (short)sample_val;
        if (is_stereo)
        {
            data[i * 2] = s_sample_val;
            data[i * 2 + 1] = s_sample_val;
        }
        else
        {
            data[i] = s_sample_val;
        }
    }
}

struct midi_setup get_midi_setup()
{
    struct midi_setup setup;
    setup.note_offset = midi_note_offset;
    setup.port = midi_port;
    setup.num_slots = num_samples;
    memcpy(setup.cc_setup, cc_mappings, sizeof(cc_mappings));
    return setup;
}
