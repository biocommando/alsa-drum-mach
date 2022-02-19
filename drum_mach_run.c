#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include "drum_mach.h"
#include "alsasnd.h"
#include "alsamidi.h"

#define CHANNELS 2
#define AUDIO_BUF_SIZE 64

#define MIDI_NOTE_ON 0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CC 0xB0

volatile int run_drum_mach_status = 0;
struct midi_setup midi_setup;

void *run_drum_mach(void *arg)
{
    printf("Start processing thread\n");
    init_snd("pcm.default", 44100);
    short audio_buf[CHANNELS * AUDIO_BUF_SIZE];
    run_drum_mach_status = 1;
    while (run_drum_mach_status == 1)
    {
        drum_mach_process_audio(audio_buf, AUDIO_BUF_SIZE, CHANNELS == 2);
        int err = write_buf_snd(audio_buf, AUDIO_BUF_SIZE);
        if (err < 0)
        {
            deinit_snd();
            usleep(1000 * 10);
            init_snd("pcm.default", 44100);
        }
    }
    printf("Processing thread done\n");
    deinit_snd();
    printf("Sound deinited\n");
    return NULL;
}

int log_midi = 0;

void do_log_midi(char *buf, int length)
{
    if (!log_midi)
      return;
    for (int i = 0; i < length; i++)
        printf("%x ", (int)(unsigned char)buf[i]);
    printf("\n");
}

void on_midi_callback(char *buf, int length)
{
    if (midi_setup.port == -1 || (buf[0] & 0x0F) == midi_setup.port)
    {
        if ((buf[0] & 0xF0) == MIDI_NOTE_ON && length >= 2)
        {
            do_log_midi(buf, 2);
            int note = buf[1];
            drum_mach_trigger(note - midi_setup.note_offset);
        }
        else if ((buf[0] & 0xF0) == MIDI_CC && length >= 3)
        {
            do_log_midi(buf, 3);
            int cc = buf[1];
            float value = buf[2] / 127.0f;
            for (int i = 0; i < NUM_CC_MAPPINGS && midi_setup.cc_setup[i].cc != -1 ; i++)
            {
                if (midi_setup.cc_setup[i].cc == cc)
                {
                    int slot = midi_setup.cc_setup[i].slot;
                    int param_id = midi_setup.cc_setup[i].param_id;
                    float min = midi_setup.cc_setup[i].min;
                    float max = midi_setup.cc_setup[i].max;
                    drum_mach_set_param(slot, param_id, (max - min) * value + min);
                }
            }
        }
    }
}

static void sig_handler(int arg)
{
    volatile int *flag = get_midi_loop_stop_flag();
    *flag = 1;
}

int main(int argc, char **argv)
{
    log_midi = 1;
    init_drum_mach(44100);

    midi_setup = get_midi_setup();

    pthread_t thread_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_create(&thread_id, &attr, &run_drum_mach, NULL);

    printf("Wait for thread to start...\n");
    while (!run_drum_mach_status)
        usleep(1000);

    printf("Run trigger code\n");
    signal(SIGINT, sig_handler);

    init_midi("hw:1");
    listen_for_midi_loop();

    run_drum_mach_status = 0;
    pthread_join(thread_id, NULL);

    printf("Thread joined\n");
    pthread_attr_destroy(&attr);

    printf("Deinit drum machine\n");
    deinit_drum_mach();
    printf("Deinit midi\n");
    deinit_midi();
    return 0;
}
