gcc --std=gnu99 -o drum_mach \
	drum_mach.c drum_mach_run.c drum_mach_conf_preproc.c filter.c \
	alsasnd.c alsamidi.c -lasound -pthread
