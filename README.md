<img src="game/icon.png" width="96" align="right" alt="">

# Soulcalibur: Recompiled (Dreamcast)

A native PC port of **Soulcalibur** (Dreamcast, USA, Namco 1999), built with
[dreamcomp](https://github.com/danielgomesvieira2000/dreamcomp): the game's SH-4 code is
statically recompiled to C++, and the Dreamcast's graphics, sound and disc hardware run as native
code around it.

**You need your own copy of the game.** This repository contains no game code or data; the port
reads your disc image at run time and refuses any release other than the one it was built from
(USA, T1401N V1.000, boot file SHA-1 `967ca1fe…c6c9`).

## Features

- Runs natively: Windows today; Linux, macOS and Android planned
- Internal resolution up to 4x (`scale` setting / `--scale`)
- Fills the window without black bars (crop), or letterbox/stretch (`fit`)
- Widescreen (anamorphic 16:9) — **experimental, currently crashes in fights; off by default**
- Saves to a per-user virtual memory card
- Keyboard and gamepad, rebindable (F1)

## Building

See dreamcomp's README for the toolchain. Then, from a dreamcomp checkout:

```powershell
git clone --recurse-submodules <this repo> ports/soulcalibur-recomp
python tools/dc.py setup ports/soulcalibur-recomp --disc "D:\discs\Soulcalibur (USA).cue"
python tools/dc.py build ports/soulcalibur-recomp
python tools/dc.py run   ports/soulcalibur-recomp --window
```

Accepted disc formats: Redump `.cue`/`.bin`, `.gdi`, `.chd`.

## Credits

- Soulcalibur © 1998, 1999 Namco Ltd. This project is not affiliated with Bandai Namco or Sega.
- [dreamcomp](https://github.com/danielgomesvieira2000/dreamcomp), built on
  [dream-recomp](https://github.com/phobos665/dream-recomp) (phobos665), which draws on
  [Flycast](https://github.com/flyinghead/flycast).
- Widescreen value from Flycast's widescreen cheat table.
- Icon: original artwork for this port (`tools/make_icon.py`), not taken from the game or its
  packaging.

## AI use

Developed with Claude Code (Anthropic); see dreamcomp's README for how.

## Licence

GPL-2.0.
