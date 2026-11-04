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

int main(int argc, char **argv) {
    ma_engine engine;

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

    struct input_event ev;

    while (1) {
	    ssize_t n = read(fd, &ev, sizeof ev);

        if (n != (ssize_t)sizeof ev) {
            if (n < 0)
                perror("read");
            break;
        }

        if (ev.type == EV_KEY && ev.value == 1) {
            if (ev.code == KEY_SPACE)
                ma_engine_play_sound(&engine, "soundpack/space.wav", NULL);
            else if (ev.code == KEY_BACKSPACE)
                ma_engine_play_sound(&engine, "soundpack/backspace.wav", NULL);
            else if (ev.code == KEY_ENTER)
                ma_engine_play_sound(&engine, "soundpack/enter.wav", NULL);
            else
                ma_engine_play_sound(&engine, "soundpack/click.wav", NULL);
        }
    }

    ma_engine_uninit(&engine);

    close(fd);

    return 0;
}
