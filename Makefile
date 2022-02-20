CC := gcc
CFLAGS := --std=gnu99 -Ofast -Wall -Wno-unused-result
LIBS := -lasound -pthread
	
DMACH_FILES := \
	drum_mach.c \
	drum_mach_run.c \
	drum_mach_conf_preproc.c \
	filter.c \
	alsasnd.c \
	alsamidi.c
	
drum_mach: wav_handler.o build_date.h
	$(CC) $(CFLAGS) $(DMACH_FILES) wav_handler.o -o $@ $(LIBS)

wav_handler.o:
	$(CC) $(CFLAGS) -c wav_handler/wav_handler.c -o $@

build_date.h:
	echo \#ifndef BUILD_DATE_H > build_date.h
	echo \#define BUILD_DATE_H >> build_date.h
	date | sed -E "s/(.*)/#define BUILD_DATE \"\0\"/" >> build_date.h
	echo \#endif  >> build_date.h

clean:
	rm -rf build_date.h wav_handler.o drum_mach
