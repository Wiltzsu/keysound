#ifndef KEYMAP_H
#define KEYMAP_H

#include <linux/input-event-codes.h>

struct slice {
    double start_ms;
    double end_ms;
};

extern const struct slice slices[KEY_MAX + 1];

#endif