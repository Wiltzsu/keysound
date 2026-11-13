#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
#define MA_ENABLE_PULSEAUDIO
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#define SOUND_DIR "/home/william/keysound/soundpack"

struct slice {
    double start_ms;
    double end_ms;
};

static const struct slice slices[KEY_MAX + 1] = {
    [KEY_SPACE] = {4353.74, 4588.84 },
    [KEY_ENTER] = {3215.96, 3424.94},
    [KEY_BACKSPACE] = {2795.10, 3030.20},
};

int main(int argc, char **argv) {
    ma_engine engine;
    // 16 sound players, so clicks can overlap when typing fast
    ma_sound voices[16];
    int sound_tracker = 0; // next voice to use

    if (argc != 2) {
        fprintf(stderr, "usage: %s /dev/input/eventN\n", argv[0]);
	    return 1;
    }

    int fd = open(argv[1], O_RDONLY);

    if (fd < 0) {
        perror("open");
        return 1;
    }

    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        fprintf(stderr, "audio init failed\n");
        close(fd);
        return 1;
    }

    // load the pack file into memory once, all 16 players share it
    for (int i = 0; i < 16; i++) {
        ma_sound_init_from_file(&engine, SOUND_DIR"/concatenated_audio.wav", MA_SOUND_FLAG_DECODE, NULL, NULL, &voices[i]);
    }

    struct input_event ev;
    ma_uint32 rate;

    // how many frames per second, needed to convert ms to frames
    ma_sound_get_data_format(&voices[0], NULL, NULL, &rate, NULL, 0);
    printf("rate: %u\n", rate);

    while (1) {
	    ssize_t n = read(fd, &ev, sizeof ev);

        if (n != (ssize_t)sizeof ev) {
            if (n < 0)
                perror("read");
            break;
        }

        if (ev.type == EV_KEY && ev.value == 1) {
            if (ev.code > KEY_MAX) {
                continue;
            }

            ma_sound *v = &voices[sound_tracker];
            sound_tracker = (sound_tracker + 1) % 16; // wrap 15 -> 0

            struct slice s = slices[ev.code];

            if (s.end_ms == 0) {
                s = slices[KEY_SPACE];
            }

            // where the sound starts and ends in the pack file
            ma_uint64 begin = s.start_ms * rate / 1000;
            ma_uint64 end = s.end_ms * rate / 1000;

            ma_data_source_set_range_in_pcm_frames(ma_sound_get_data_source(v), begin, end);

            // rewind to the start of the slice, then play
            ma_sound_seek_to_pcm_frame(v, 0);
            ma_sound_start(v);
        }
    }

    ma_engine_uninit(&engine);

    close(fd);

    return 0;
}
