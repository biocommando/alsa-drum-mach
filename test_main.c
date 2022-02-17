#include "drum_mach.h"
#include <stdio.h>

void create_output(int stereo)
{
    char fname[16];
    sprintf(fname, "out_%c.raw", stereo ? 's' : 'm');

    init_drum_mach(44100);
    short buf[128];
    FILE *fout = fopen(fname, "wb");

    int samples_per_frame = stereo ? 64 : 128;

    for (int i = 0; i < 44100; i += samples_per_frame)
    {
        if (i == 0)
            drum_mach_trigger(0);
        if (i == 140 * 128)
        {
            drum_mach_trigger(0);
        }
        if (i == 141 * 128)
        {
            drum_mach_set_param(0, PARAM_ID_SPEED, 0.5);
            drum_mach_trigger(0);
        }
        if (i == 172 * 128)
        {
            drum_mach_trigger(0);
            drum_mach_trigger(1);
        }
        drum_mach_process_audio(buf, samples_per_frame, stereo);
        fwrite(buf, sizeof(short), 128, fout);
    }
    fclose(fout);

    deinit_drum_mach();
}

int main(int argc, char **argv)
{
    printf("Run for mono\n");
    create_output(0);
    printf("Run for stereo\n");
    create_output(1);
}
