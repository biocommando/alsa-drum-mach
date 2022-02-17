#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "drum_mach.h"
#include "alsasnd.h"

#define CHANNELS 2
#define AUDIO_BUF_SIZE 32

volatile int run_drum_mach_status = 0;

void *run_drum_mach(void *arg)
{
    printf("Start processing thread\n");
    init_snd("pcm.default", 44100);
    short audio_buf[CHANNELS * AUDIO_BUF_SIZE];
    run_drum_mach_status = 1;
    while (run_drum_mach_status == 1)
    {
        drum_mach_process_audio(audio_buf, AUDIO_BUF_SIZE, CHANNELS == 2);
        write_buf_snd(audio_buf, AUDIO_BUF_SIZE);
    }
    printf("Processing thread done\n");
    deinit_snd();
    return NULL;
}

int main(int argc, char **argv)
{
    init_drum_mach(44100);

    pthread_t thread_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_create(&thread_id, &attr, &run_drum_mach, NULL);

    printf("Wait for thread to start...\n");
    while(!run_drum_mach_status) usleep(1000);

    printf("Run trigger code\n");
    while(1)
    {
        int trg = 100;
        printf("Give trigger (-1 quits):\n");
        scanf("%d", &trg);
        if (trg == -1)
           break;
        drum_mach_trigger(trg);
    }
    run_drum_mach_status = 0;
    void *res;
    pthread_join(thread_id, res);

    pthread_attr_destroy(&attr);

    deinit_drum_mach();
}
