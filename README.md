This is a very basic audio application using ALSA audio drivers.
It's a PCM sample based drum machine that reads the samples based on
dynamically loaded configurations. The drum machine is operated using MIDI.

Supports also setting basic parameters for sample playback as well as
mapping CCs to them.

I have tested and run this on 1st gen Raspberry PI and it seemed to work quite nicely.
Some optimizations (e.g. for HW parameters) etc. need still to be made. The CC mapping
has not been tested yet.

# Configuration file formats

## path_config.txt

Must exist at the same level as the main application. Contains a single line that
is the path in which the samples are located. Must be terminated with path separator.
Example contents:
```
/audio/drum1/samples/
``` 

## sample_data_config.txt

Must exist at the same level as sample data.
Contents:
```
num_slots midi_note_offset midi_port
sample_name number_of_samples speed volume speed_cc volume_cc
sample_name number_of_samples speed volume speed_cc volume_cc
...
```
`num_slots` = number of samples to load, one to each sample slot

(optional) `midi_note_offset` = offset at which MIDI note the first slot is. Defaults to 0.

(optional) `midi_port` = which midi port is listened to. -1 means all. Defaults to -1.

`sample_name` = the full name of the sample file. Sample file format is raw binary
                with signed 16-bit LE samples on a single channel at sample rate 44.1 kHz.

`number_of_samples` = the number of samples to load from the file

(optional) `speed` = speed multiplier, e.g. 1.5 plays the sample 1.5 times faster. Defaults to 1.

(optional) `volume` = volume multiplier (linear), e.g. 0.5 means that sample is played with half
                      the volume. Defaults to 0.5.

(optional) `speed_cc` = CC number that is mapped to speed (range 0 to 1). Defaults to -1 (= not mapped).

(optional) `volume_cc` = CC number that is mapped to volume (range 0 to 1). Defaults to -1 (= not mapped).
