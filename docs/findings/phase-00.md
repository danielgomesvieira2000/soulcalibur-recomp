# Phase 00 — survey (2026-10-05)

- Disc: Redump cue/bin, 3 tracks. IP.BIN: `SEGA SEGAKATANA`, `SEGA ENTERPRISES`, device
  `5B21 GD-ROM1/1`, area `U`, peripherals `07BBA10` (VGA, Puru Puru, VMU, analog triggers/stick),
  product `T1401N`, version `V1.000`, date `19990730`, boot `1ST_READ.BIN`, title `SOULCALIBUR`.
- Boot file 3,679,460 bytes, SHA-1 `967ca1fe2e8c7df57a5de1e83d939bed483cc6c9`, stored plain.
- Not Windows CE. Katana SDK (Shinobi/Ninja/Kamui/sd).
- No decompilation or PC port found (research 2026-10-05). Deecy (MIT emulator) reportedly runs it.
  Flycast has a widescreen value for T1401N.
- Engine evaluation: translated 6,221 functions, 527,534 instructions, 623 not lowered, 43
  switch tables. Ran to Character Select on first try once 4 relocations were added.
