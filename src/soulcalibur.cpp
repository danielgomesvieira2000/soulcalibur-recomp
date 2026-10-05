// Soulcalibur (USA, T1401N): what this port tells dreamcomp, and its game-specific enhancements.
// Addresses are guest addresses in the boot executable this port was translated from
// (game/soulcalibur.toml, sha1_1st_read); docs/GAME-INTERNALS.md says how each was found.
#include <cstdint>
#include <cstdio>

#include "dream/runtime/system.h"
#include "dreamcomp/hook.h"
#include "dreamcomp/port.h"
#include "dreamcomp/settings.h"

namespace {

// Horizontal scale of the 3D projection (float). The game recomputes it, so the value is
// re-asserted every vblank while widescreen is on. 0.75 squeezes a 16:9 view into the 640x480
// framebuffer (anamorphic); the presenter stretches it back. Source of the address and value:
// Flycast's built-in widescreen cheat for T1401N (core/cheats.cpp). The HUD is stretched with
// it -- see docs/GAME-INTERNALS.md "Widescreen".
constexpr std::uint32_t kProjectionScaleX = 0x8C266C28;
constexpr float kWidescreenScaleX = 0.75f;

bool g_patched = false;
float g_original = 0.0f;

void widescreen(dream::System& sys, bool enabled) {
    auto& m = sys.memory;
    if (enabled) {
        const float now = dreamcomp::read_f32(m, kProjectionScaleX);
        if (!g_patched || now != kWidescreenScaleX) {
            if (now != kWidescreenScaleX)
                g_original = now;  // the game's own value, restored when switched off
            dreamcomp::write_f32(m, kProjectionScaleX, kWidescreenScaleX);
            g_patched = true;
        }
    } else if (g_patched) {
        dreamcomp::write_f32(m, kProjectionScaleX, g_original);
        g_patched = false;
    }
}

const dreamcomp::PortInfo kInfo = [] {
    dreamcomp::PortInfo p;
    p.id = "soulcalibur";
    p.title = "Soulcalibur";
    p.widescreen = &widescreen;
    p.widescreen_aspect = 16.0f / 9.0f;
    p.widescreen_anamorphic = true;
    return p;
}();
dreamcomp::RegisterPort g_register(kInfo);

}  // namespace
