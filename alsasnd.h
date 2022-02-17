#ifndef ALSA_SND_WRAP_H
#define ALSA_SND_WRAP_H

void init_snd(const char *dev);
void deinit_snd();
void write_buf_snd(short *buf, int count);

#endif