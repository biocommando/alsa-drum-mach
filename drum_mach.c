#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "drum_mach.h"

// 128 midi notes so no point in defining more than that
#define MAX_NUM_SAMPLES 128

struct sample_data
{
    short *buf;
    int size;
    float pos;
    float speed;
    float vol;
};

struct sample_data sample_data_arr[MAX_NUM_SAMPLES];
int init_done = 0;
int num_samples;

void init_drum_mach(int sample_rate)
{
    if (init_done)
        return;

    FILE *fbin, *fconfig;
    
    double speed_mult = 44100.0 / sample_rate;

    fconfig = fopen("path_config.txt", "r");
    if (!fconfig)
    {
        printf("Could not open path_config.txt\n");
        return;
    }
    char sample_data_path[256];
    fscanf(fconfig, "%s", sample_data_path);
    fclose(fconfig);

    char config_path[280];
    sprintf(config_path, "%ssample_data_config.txt", sample_data_path);
    printf("Load config from '%s'\n", config_path);
    fconfig = fopen(config_path, "r");
    if (!fconfig)
    {
        printf("Could not open file!\n");
        return;
    }

    char readbuf[256];
    fgets(readbuf, 256, fconfig);
    num_samples = MAX_NUM_SAMPLES;
    sscanf(readbuf, "%d", &num_samples);
    if (num_samples <= 0 || num_samples >= MAX_NUM_SAMPLES)
    {
        printf("Erroneous number of samples read from config file (unparsed value '%s')\n", readbuf);
        fclose(fconfig);
        return;
    }
    printf("Number of slots in use: %d\n", num_samples);

    for (int i = 0; i < num_samples; i++)
    {
        int sz = 0;
        char sample_name[256];
        float speed = 1;
        float vol = 0.5;
        fgets(readbuf, 256, fconfig);
        sscanf(readbuf, "%s %d %f %f", sample_name, &sz, &speed, &vol);
        
        sample_data_arr[i].buf = NULL;
        sample_data_arr[i].size = sz;
        sample_data_arr[i].pos = sz;
        sample_data_arr[i].speed = speed * speed_mult;
        sample_data_arr[i].vol = vol;
     
        char fname[1024];
        sprintf(fname, "%s%s", sample_data_path, sample_name);
        
        printf("Load sample '%s' (length %d) to slot %d\n", fname, sz, i);

        fbin = fopen(fname, "rb");
        
        if (sz)
        {
            sample_data_arr[i].buf = malloc(sz * sizeof(short));
            fread(sample_data_arr[i].buf, sizeof(short), sz, fbin);
        }
        fclose(fbin);
    }
    fclose(fconfig);
    init_done = 1;
}

void deinit_drum_mach()
{
    if (!init_done)
        return;
    printf("Free drum samples\n");
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
    if (idx >= 0 && idx < num_samples)
    {
        if (param == PARAM_ID_SPEED)
            sample_data_arr[idx].speed = param_val;
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
