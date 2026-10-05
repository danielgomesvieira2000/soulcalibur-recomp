# Soulcalibur (USA) — game internals

Facts about T1401N V1.000 as measured by this port. Addresses are load-address spelling
(`0x8C......`); the translator's link space is the same RAM at `0x0C......`.

## Disc

| | |
|---|---|
| Layout | GD-ROM: track 1 data (300 sectors), track 2 audio (LBA 450), track 3 data at LBA 45000 (504,150 sectors) |
| Files | 88 in the root. `1ST_READ.BIN` (code, 3.6 MB); `*.DAT` archives (`HUMAN.DAT` 78 MB characters, `HOPEN.DAT` 52 MB, `STAGE.DAT` 42 MB, `KENDING/KGALLERY/KMISSION/ENBU/CINIT/CDATA/VCSEL`); `0GDTEX.PVR`; music as split stereo streams `GBGMxxL/R.P16` and `SBGMxxL/R.P16` (also `.P08`/`.P04` variants) |
| Code files | one: no overlays found (no other `.BIN`) |
| ISO9660 extents | absolute LBAs (not FAD) on the Redump image |

## Boot

- Loaded at `0x8C010000`; entry `0x8C010000`. Strings: "Lib Handle Start", Shinobi 1.43
  (Jul 23 1999), Ninja (Apr 9 1999), KAMUI 1.06, sd 1.00.18, gdFs 1.07.
- Katana copies exception/interrupt stubs to `0x8C00007C`, `0x8C000100`, `0x8C000400`,
  `0x8C000600` (VBR = `0x8C000000`); configured as `[[relocations]]` (playbook T1).
- First 1800 frames with the standard script: 11,805 interrupts, 4,894 GD-ROM syscalls, 1,670
  renders, 0 untranslated targets after the relocations.
- Untranslated target reached ~frame 3000 in a fight: `0x0C019FC0` (2 calls) — to seed.

## Sound

- The ARM7 driver keys voices with 32-bit stores to AICA channel registers (engine fix, playbook
  T2). Audio starts at the first Start press (~frame 600); silent before that, which is plausible.
- The SH-4 talks to the driver only through sound RAM (4 SH-4 AICA register writes per run).

## Widescreen

- `0x8C266C28` (float) = horizontal projection scale; Flycast writes `0.75` (`0x3F400000`) every
  frame for 16:9. The image holds 0 there (BSS); the game sets it at run time.
- With the value forced from boot, the translated build faults at frame 2272 (`pc 0x0C02DD74`,
  `pr 0x0C0224E6`) on the standard script; fully interpreted it runs clean → emitter defect,
  under investigation.

## HUD

- 2D HUD (health bars, names, timer, win orbs, "BATTLE 1", stage name/track title) is drawn as TA
  sprites (PCW `a0800009` / `a08c0009` / `a0840009`). Per frame, the HUD sprites share one or two
  exact depths (1/w 0.113-0.207, following the scene) plus an overlay layer at 2000/1851.85; 3D
  particle sprites (chandelier flames, dust) each have their own depth (up to 0.41). Measured on
  17 frames between 2000 and 6800 of a scripted arcade run (dreamcomp docs/HUD.md).

## Saves

- Needs 12 free blocks on the VMU in controller port A1. With a fresh formatted card, pressing A on
  the create prompt (~frame 300) writes the save ("Successfully saved SOULCALIBUR game data file");
  the next boot reads it (27 block reads, 6 VMU screen writes) and shows "Successfully loaded".

## Display

- 640x480, framebuffers at `0x200000` / `0x600000` in VRAM (RGB888).
- Intro (frames 60-200): zooming "namco" logo, "Produced by namco / THE LEGEND WILL NEVER DIE! /
  SOULCALIBUR for Dreamcast", over a green tiled background whose tiles appear in a checkerboard
  with gaps. Not established whether the gaps are a rendering defect or the original effect.
