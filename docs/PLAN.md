# Soulcalibur port — plan

Autonomous session (Daniel, 2026-10-05: "Go and start now, autonomously"): recommended option
taken at each decision and logged below.

## Standing constraints

- No game data in the repo (dreamcomp `docs/LEGAL.md`); `audit.py` pre-commit hook installed.
- Accuracy first: one verified change at a time; enhancements behind settings, on once verified.
- No black bars (default fit = crop). High frame rate never by changing game logic rate.
- Windows first, then Linux, macOS, Android.

## Phases

| Phase | Entry gate | Exit criteria (visible behaviour) | Status |
|---|---|---|---|
| 00 Survey | disc present | identity, SDK, files recorded (`findings/phase-00.md`) | done |
| 01 Boot | config builds | menus reachable; 0 untranslated targets / 0 unmapped over 1800 frames | done |
| 02 Audio | 01 | music + effects audible | done by measurement (key-ons, RMS); **Daniel to listen** |
| 03 Full play | 02 | arcade run to the ending with no fault; versus; saves load | in progress: fights reached (frame ~1700-3500), seed `0x0C019FC0` pending |
| 04 Widescreen | 03 for the fight path | 16:9 in fights, no fault over 3600 frames; HUD plan | 3700 frames clean after the engine entry-poll fix (2026-10-05); HUD stretched (anamorphic); **Daniel to judge** |
| 05 Polish | 03 | intro tile gaps checked against a reference (defect or original?); settings UI | open |

## Decisions

| Date | Decision | Options | Why | By |
|---|---|---|---|---|
| 2026-10-05 | Target T1401N V1.000 (Daniel's disc) | — | only disc available | — |
| 2026-10-05 | Widescreen via Flycast's per-title value (anamorphic) first | Hor+ via hooks | proven value, cheap; HUD stretch accepted for a first step | Claude (autonomous) |

## Verified checkpoints

| Date | Port commit | dreamcomp commit | What was confirmed, by whom |
|---|---|---|---|
| — | — | — | none yet by Daniel |
