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

## Frame wait (idle skip)

Found with the host profiler (2026-10-06): `fn_0c2246a0` was the innermost game function in 25 % of
the game thread's busy time, and guest-PC sampling (`--sample 20000`) put 46.9 % of emulated time
in its loop at 0x0c2247f8-0x0c22481e:

```
while (*(u32*)0x8C379E38) {                    // cleared by an interrupt
    (*(void(**)(u32))0x8C379D34)(*(u32*)0x8C379D38);   // = 0x8C22346C: rts; nop
    if (*(u32*)0x8C379E34 > *(u32*)0x8C379CF0 + *(u32*)0x8C379CF4 + 1) break;  // timeout
}
```

A pass is 22 cycles from the callback's entry check to the next (back-edge check at +18) and
changes nothing but the clock. The entry hook `idle_wait` on 0x8C22346C (called with pr =
0x0C224800 and the flag set, previous call exactly 22 cycles earlier) advances the clock by the
whole passes that end before the next scheduled event, so the event lands on the same check and
cycle as when spinning. Verified bit-identical: fight scenario audio and four screenshots.
Effect, real-speed fight on battery: game thread busy 77.9 % -> 67.4 % of wall time, host time
per frame p50 11.87 -> 11.00 ms, p90 18.0 -> 16.9 ms. `DREAMCOMP_NO_IDLE_SKIP=1` turns it off.
Since 2026-10-06 the hook forwards to dreamcomp's shared `IdleWait` (include/dreamcomp/idle.h);
re-verified bit-identical (fight: audio, four screenshots; 60.4 -> 52.3 G game-thread cycles).

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

## HUD overrides (game/hud_overrides.ini)

13 entries promoted 2026-10-07 from Daniel's first F1 editor session (`tools/hud_promote.py`).
Effect against no overrides (`scenario.py --compare`, Expanded): fight HUD 0.6 % of pixels;
the VS / stage-intro screen (fight frame 1500, menus 1250 and 1450) 22-36 %: both portraits and
their name blocks anchored to the centre (their 4:3 positions) instead of the edges, the
opponent's portrait now against "VS". Three entries cover large areas (8,42-381,302;
368,208-610,442; 28,48-270,272, all `center`), saved while some outlines spanned most of the
screen. Kept as Daniel promoted them; review in the editor (Space pauses on the VS screen).

Second promotion 2026-10-07 (14 entries): `rect=-2,-2,642,482 anchor=stretch tcw=0x00000000,0x28e6d200`.
One editor click hit an element outlined as the whole screen; that created a whole-screen
override and every later click on a piece with those textures cycled the same entry. Effect
(against the first promotion): untextured 2D pieces and texture 0x28E6D200 are stretched
everywhere -- the main-menu box frame widens around unstretched text, "TIME UP!!" is stretched,
the VS screen's name bars span the width. Flagged to Daniel; the editor flaw is being fixed.

