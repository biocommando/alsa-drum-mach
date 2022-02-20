
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <alloca.h>
#include <alsa/asoundlib.h>

static snd_rawmidi_t *input = NULL, **inputp;

#define error printf

void init_midi(const char *port_name)
{
    int err;
    inputp = &input;
    if ((err = snd_rawmidi_open(inputp, NULL, port_name, SND_RAWMIDI_NONBLOCK)) < 0)
    {
        error("cannot open port \"%s\": %s", port_name, snd_strerror(err));
        return;
    }
}

void deinit_midi()
{
    snd_rawmidi_close(input);
}

volatile int midi_loop_stop_flag = 0;
volatile int *get_midi_loop_stop_flag()
{
    return &midi_loop_stop_flag;
}

extern void on_midi_callback(char *, int);

void listen_for_midi_loop()
{
    int err;
    int npfds;
    struct pollfd *pfds;
    npfds = snd_rawmidi_poll_descriptors_count(input);
    pfds = alloca(npfds * sizeof(struct pollfd));
    snd_rawmidi_poll_descriptors(input, pfds, npfds);
    while (1)
    {

        char buf[256];
        int i, length;
        unsigned short revents;

        err = poll(pfds, npfds, 200);
        if (midi_loop_stop_flag)
            break;
        if (err <= 0)
            continue;
        if ((err = snd_rawmidi_poll_descriptors_revents(input, pfds, npfds, &revents)) < 0)
        {
            error("cannot get poll events: %s", snd_strerror(errno));
            break;
        }
        if (revents & (POLLERR | POLLHUP))
            break;
        if (!(revents & POLLIN))
            continue;
        err = snd_rawmidi_read(input, buf, sizeof(buf));
        if (err == -EAGAIN)
            continue;
        if (err < 0)
        {
            error("cannot read from port: %s", snd_strerror(err));
            break;
        }
        length = err;
        if (length == 0)
            continue;

        for (i = 0; i < length; i++)
        {
            if (buf[i] & 0x80)
                on_midi_callback(&buf[i], length - i);
        }
    }
}
