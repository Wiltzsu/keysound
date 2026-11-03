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
# Put a short click.wav in soundpack/ (your own file or a Mechvibes pack slice).
```

Update the wav path in `keysound.c` if you use something other than `soundpack/click.wav`.

## Build

```bash
gcc -Wall -Wextra -o keysound keysound.c -lpthread -ldl -lm -lpulse
```

## Run

From this directory (relative wav path):

```bash
./keysound /dev/input/eventN
```

Find the device: `sudo apt install evtest && sudo evtest` - pick your keyboard.

**keyd:** if the physical keyboard is grabbed, use the **keyd virtual keyboard** node instead (often `/dev/input/event11`). Check with `evtest`.

Do not run with `sudo` - audio goes to your user session.

## Notes

- `keysound.c` enables miniaudio's PulseAudio backend so audio routes to normal desktop output (not silent HDMI ALSA).
- Sound packs from [mechvibes.com](https://mechvibes.com/) are third-party; keep them out of git (`soundpack/` is gitignored).
