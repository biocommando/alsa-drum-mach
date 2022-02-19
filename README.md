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
The configuration is based on commands and command parameters.
The command syntax is:
```
C parameter1 parameter2 ...
```
Where C is one of the allowed commands.

The following commands are available:

**Comment**
```
. Comment text
```

**Variable definition**
```
: name value
```

`name` = name of the variable (max 15 characters)

`value` = value of the variable (max 15 characters)

The variables work so that before any lines are processed variable substitution is
done first: all `{name}` tags are replaced with the respective value (or empty string
if variable is not defined).

**Include variable definitions**
```
I file_name is_local
```

`file_name` = file name to include (note that same file must not be included twice)

(optional) `is_local` = if 1, the file is searched from the sample directory, if 0 file is searched
             from executable directory. Defaults to 1.

**Number of slots**
```
# num_slots
```

`num_slots` = number of samples to load, one to each sample slot

**Midi config**
```
M midi_note_offset midi_port
```

(optional) `midi_note_offset` = offset at which MIDI note the first slot is. Defaults to 0.

(optional) `midi_port` = which midi port is listened to. -1 means all. Defaults to -1.

**Load sample**

```
L sample_name number_of_samples
```

`sample_name` = the full name of the sample file. Sample file format is raw binary
                with signed 16-bit LE samples on a single channel at sample rate 44.1 kHz.

`number_of_samples` = the number of samples to load from the file


**Set parameter**
```
P slot param_id value
```

`slot` = sample slot that is affected (-1 for global parameters)

`param_id` = parameter id:
```
param_id    default    description
1           1          playback speed
2           0.5*       volume (linear)
10          0          global filter enable (0/1)
11          1          global filter cutoff
12          0          global filter resonance

* global volume defaults to 1
```

`value` = value as decimal

**CC mapping**
```
C slot param_id cc min max
```

`slot` and `param_id` are same as in `set parameter` command.

`cc` = MIDI CC control to which this parameter is mapped

`min`, `max` = the parameter will get values between `min` - `max` when CC messages are sent.

