#include "drum_mach.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    init_drum_mach();

    FILE *fout = fopen("out.raw", "wb");
    short mono_buf[128];
    short stereo_buf[128 * 2];
    for (int i = 0; i < 44100; i += 128)
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
        //drum_mach_process_audio(mono_buf, 128, 0);
        drum_mach_process_audio(stereo_buf, 128, 1);
        fwrite(stereo_buf, sizeof(short), 128 * 2, fout);
    }
    fclose(fout);

    deinit_drum_mach();
}
