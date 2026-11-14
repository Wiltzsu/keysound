# keysound

Play a click sound on keyboard key presses (Linux evdev + [miniaudio](https://github.com/mackron/miniaudio)).

## Requirements

- Linux with evdev (`/dev/input/event*`)
- Build: `gcc`, `libpulse-dev` (PipeWire/PulseAudio playback)
- User in the **`input`** group (log out/in after `sudo usermod -aG input "$USER"`)

## Setup

```bash
curl -fsSL -o miniaudio.h \
  https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h

mkdir -p soundpack
# Put the Mechvibes pack files in soundpack/ (concatenated_audio.wav + config.json).
```

The per-key timings in `keymap.c` match one specific pack (`config.json` in `soundpack/`). A different pack needs a regenerated table.

## Build

```bash
gcc -Wall -Wextra -o keysound keysound.c keymap.c -lpthread -ldl -lm -lpulse
```

## Run

```bash
./keysound [-d soundpack_dir] /dev/input/eventN
```

`-d` sets the folder that holds `concatenated_audio.wav` (default: `SOUND_DIR` in `keysound.c`).

Find the device: `sudo apt install evtest && sudo evtest` - pick your keyboard.

**keyd:** if the physical keyboard is grabbed, use the **keyd virtual keyboard** node instead (often `/dev/input/event11`). Check with `evtest`.

Do not run with `sudo` - audio goes to your user session.

## Notes

- `keysound.c` enables miniaudio's PulseAudio backend so audio routes to normal desktop output (not silent HDMI ALSA).
- Sound packs from [mechvibes.com](https://mechvibes.com/) are third-party; keep them out of git (`soundpack/` is gitignored).
