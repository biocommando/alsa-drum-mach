#ifndef ALSA_SND_WRAP_H
#define ALSA_SND_WRAP_H

void init_snd(const char *dev, int sample_rate);
void deinit_snd();
int write_buf_snd(short *buf, int count);

#endif
