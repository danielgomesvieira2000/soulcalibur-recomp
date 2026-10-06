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
// re-asserted every vblank while widescreen is on. (4/3) / aspect squeezes the wider view into the
// 640x480 framebuffer (anamorphic): 0.75 at 16:9 -- the value of Flycast's widescreen cheat for
// T1401N (core/cheats.cpp) -- 0.571 at 21:9. The engine renders into a target of the same shape,
// so the squeeze is undone at full resolution. The HUD is stretched with it: docs/GAME-INTERNALS.md.
constexpr std::uint32_t kProjectionScaleX = 0x8C266C28;

bool g_patched = false;
float g_original = 0.0f;

void widescreen(dream::System& sys, float aspect) {
    auto& m = sys.memory;
    const bool enabled = aspect > 4.0f / 3.0f + 0.01f;
    const float want = (4.0f / 3.0f) / aspect;
    if (enabled) {
        const float now = dreamcomp::read_f32(m, kProjectionScaleX);
        if (!g_patched || now != want) {
            if (!g_patched || (now != want && now != 0.0f && now > want + 0.001f))
                g_original = now;  // the game's own value, restored when switched off
            dreamcomp::write_f32(m, kProjectionScaleX, want);
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
    p.widescreen_anamorphic = true;
    p.max_aspect = 32.0f / 9.0f;
    // HUD (health bars, names, timer, banners) is drawn as sprites sharing one depth per frame
    // (0.11-0.21, moving with the scene) plus an overlay layer at 1/w = 2000; 3D particles are
    // sprites too but each at its own depth. Measured on 17 fight frames: docs/GAME-INTERNALS.md.
    p.hud.enabled = true;
    // Character select draws its portraits as flat polygons, and the selected one and its frame
    // each at a depth of their own (1/w 3 and 5.01); no 3D geometry comes closer than about 0.3,
    // so anything at 1/w >= 1 is 2D.
    p.hud.polygons = true;
    p.hud.overlay_z = 1.0f;
    p.players = 2;
    return p;
}();
dreamcomp::RegisterPort g_register(kInfo);

}  // namespace
