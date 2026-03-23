# alu81a-rgb

CLI RGB controller for the Dark Project Terra Nova / Nostra (ALU81A) keyboard.
Talks directly to the keyboard over hidraw - no daemons, no dependencies, instant.

```
alu81a-rgb --color orange
alu81a-rgb --color red --mode breathing -b 80
alu81a-rgb --hex 00aaff --mode pixel-fractal --speed 180
alu81a-rgb --off
```

## Requirements

- Linux with hidraw support (standard everywhere)
- Dark Project Terra Nova / Nostra keyboard - `VID=0x342D PID=0xE40B`
- Read/write access to `/dev/hidraw*` - handled by the udev rule (see [Permissions](#permissions))

## Installation

### Arch Linux

```bash
yay -S alu81a-rgb
```

### From source

```bash
git clone https://github.com/kotoxik/alu81a-rgb
cd alu81a-rgb
bash install.sh
```

The script checks for `gcc` and `make`, builds, installs, reloads udev, and adds your user to the `input` group.

### Manual

```bash
make
sudo make install
```

Then reload udev and re-plug the keyboard:

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

## Permissions

The udev rule gives the `input` group rw access to the keyboard's hidraw node, so you never need `sudo`.

Add your user to the group:

```bash
sudo usermod -aG input $USER
# then log out and back in, or:
newgrp input
```

## Usage

```
alu81a-rgb [COLOR] [EFFECT] [OPTIONS]
```

### Color

| Flag | Format | Example |
|---|---|---|
| `--color NAME` | Named color | `--color red` |
| `--hex RRGGBB` | Hex string | `--hex ff4400` or `--hex #ff4400` |
| `--rgb R,G,B` | 0-255 each | `--rgb 255,70,0` |
| `--hsl H,S,L` | H: 0-360  S/L: 0-100 | `--hsl 30,100,50` |

Run `alu81a-rgb --list-colors` to see all named colors.

### Effect

| Flag | Description | Default |
|---|---|---|
| `-m`, `--mode NAME\|ID` | Effect mode | `solid` |
| `-s`, `--speed 0-255` | Animation speed (255 = fastest) | `255` |
| `-b`, `--brightness 0-100` | Global brightness % | `100` |

Run `alu81a-rgb --list-modes` to see all 45 modes.

### Other flags

```
--off            Turn off all LEDs
--device PATH    Override auto-detected hidraw device
--list-modes     Print all effect modes and exit
--list-colors    Print all named colors and exit
-v, --verbose    Show device path and HID payload
-h, --help       Show help and exit
```

### Examples

```bash
alu81a-rgb --color white
alu81a-rgb --hex ff0000
alu81a-rgb --color blue --mode breathing
alu81a-rgb --color red  --mode pixel-fractal --speed 120
alu81a-rgb --hsl 200,100,50 --mode hue-wave --speed 200
alu81a-rgb --color white -b 20
alu81a-rgb --off
```

## Modes

| ID | Slug | Name |
|----|------|------|
| 0 | `disable` | Disable |
| 1 | `direct` | Direct Control |
| 2 | `solid` | Solid Color |
| 3 | `alphas-mods` | Alphas Mods |
| 4 | `gradient-ud` | Gradient Up Down |
| 5 | `gradient-lr` | Gradient Left Right |
| 6 | `breathing` | Breathing |
| 7 | `band-sat` | Band Sat |
| 8 | `band-val` | Band Val |
| 9 | `band-pinwheel-sat` | Band Pinwheel Sat |
| 10 | `band-pinwheel-val` | Band Pinwheel Val |
| 11 | `band-spiral-sat` | Band Spiral Sat |
| 12 | `band-spiral-val` | Band Spiral Val |
| 13 | `cycle-all` | Cycle All |
| 14 | `cycle-lr` | Cycle Left Right |
| 15 | `cycle-ud` | Cycle Up Down |
| 16 | `rainbow-chevron` | Rainbow Moving Chevron |
| 17 | `cycle-out-in` | Cycle Out In |
| 18 | `cycle-out-in-dual` | Cycle Out In Dual |
| 19 | `cycle-pinwheel` | Cycle Pinwheel |
| 20 | `cycle-spiral` | Cycle Spiral |
| 21 | `dual-beacon` | Dual Beacon |
| 22 | `rainbow-beacon` | Rainbow Beacon |
| 23 | `rainbow-pinwheels` | Rainbow Pinwheels |
| 24 | `raindrops` | Raindrops |
| 25 | `jellybean-raindrops` | Jellybean Raindrops |
| 26 | `hue-breathing` | Hue Breathing |
| 27 | `hue-pendulum` | Hue Pendulum |
| 28 | `hue-wave` | Hue Wave |
| 29 | `typing-heatmap` | Typing Heatmap |
| 30 | `digital-rain` | Digital Rain |
| 31 | `solid-reactive-simple` | Solid Reactive Simple |
| 32 | `solid-reactive` | Solid Reactive |
| 33 | `solid-reactive-wide` | Solid Reactive Wide |
| 34 | `solid-reactive-multiwide` | Solid Reactive Multiwide |
| 35 | `solid-reactive-cross` | Solid Reactive Cross |
| 36 | `solid-reactive-multicross` | Solid Reactive Multicross |
| 37 | `solid-reactive-nexus` | Solid Reactive Nexus |
| 38 | `solid-reactive-multinexus` | Solid Reactive Multinexus |
| 39 | `splash` | Splash |
| 40 | `multisplash` | Multisplash |
| 41 | `solid-splash` | Solid Splash |
| 42 | `solid-multisplash` | Solid Multisplash |
| 43 | `pixel-rain` | Pixel Rain |
| 44 | `pixel-fractal` | Pixel Fractal |

## Shell completions

Installed automatically by `make install` for Fish, Zsh, and Bash.

Fish and Bash work immediately. For Zsh with a manual install to `/usr/local`, add this to your `.zshrc` before `compinit`:

```zsh
fpath=(/usr/local/share/zsh/site-functions $fpath)
```

Tab completion covers color names (with hex values), mode names (with descriptions), all flags, and `/dev/hidraw*` paths.

## Protocol

See the reverse-engineered protocol documentation:
https://github.com/IlyaKotomin/terra-nova-nostra-rgb-protocol

## License

MIT
