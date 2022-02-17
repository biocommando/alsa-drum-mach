gcc --std=gnu99 -o drum_mach drum_mach.c drum_mach_run.c alsasnd.c alsamidi.c -lasound -DSAMPLE_DATA_PATH=\"TR-505/\" -pthread
