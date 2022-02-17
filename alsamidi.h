#ifndef ALSA_MIDI_WRAP_H
#define ALSA_MIDI_WRAP_H
void listen_for_midi_loop();
volatile int *get_midi_loop_stop_flag();
void deinit_midi();
void init_midi(const char *port_name);
#endif
