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

int main(int argc, char **argv) {
    ma_engine engine;
    // voice pool: one ma_sound plays one thing at a time, 16 lets fast typing overlap
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

    // DECODE: wav is decoded to PCM once, all voices share that buffer
    for (int i = 0; i < 16; i++) {
        ma_sound_init_from_file(&engine, SOUND_DIR"/concatenated_audio.wav", MA_SOUND_FLAG_DECODE, NULL, NULL, &voices[i]);
    }

    struct input_event ev;
    ma_uint32 rate;

    // engine rate (48000 here), not the wav's 44100: DECODE resamples. don't hardcode
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
            ma_sound *v = &voices[sound_tracker];
            sound_tracker = (sound_tracker + 1) % 16; // wrap 15 -> 0

            // ms -> frames. TEMP: Space slice from config.json for every key
            ma_uint64 begin = 4353.74 * rate / 1000;
            ma_uint64 end = 4588.84 * rate / 1000;

            ma_data_source_set_range_in_pcm_frames(ma_sound_get_data_source(v), begin, end);

            // with a range set, 0 = start of the slice. start() alone won't rewind
            // a voice that hasn't reached its end
            ma_sound_seek_to_pcm_frame(v, 0);
            ma_sound_start(v);
        }
    }

    ma_engine_uninit(&engine);

    close(fd);

    return 0;
}
