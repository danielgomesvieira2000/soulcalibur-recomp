// Soulcalibur (USA, T1401N): what this port tells dreamcomp, and its game-specific enhancements.
// Addresses are guest addresses in the boot executable this port was translated from
// (game/soulcalibur.toml, sha1_1st_read); docs/GAME-INTERNALS.md says how each was found.
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "dream/runtime/system.h"
#include "dreamcomp/hook.h"
#include "dreamcomp/idle.h"
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

// The frame-wait loop (fn_0c2246a0, 0x0c2247f8-0x0c22481e): while the flag at 0x8C379E38 is set,
// call the function pointer at 0x8C379D34 -- the empty function 0x8C22346C -- and check a
// frame-count timeout; an interrupt clears the flag. The game spends ~47 % of its emulated time
// here in a fight, and every pass costs the host an indirect-call lookup and several reads
// (tools/profile.py, docs/GAME-INTERNALS.md). A pass is 22 cycles from this callback's entry
// check (A) to the next, with the loop's back-edge check (B) at +18, and changes nothing but the
// clock. Called from that loop with the flag still set, the callback skips the whole passes that
// end before the next scheduled event (the largest k with A_k still short of it) by advancing the
// clock k * 22: the pass in which the event falls then runs for real, and the event is delivered
// at the same check, on the same cycle, as when spinning -- interrupts, sound, timers and the
// game's own timing are bit-identical (scenario audio and screenshots compared). It acts only
// when the previous call came exactly one pass earlier, i.e. inside the steady loop
// (dreamcomp/idle.h). DREAMCOMP_NO_IDLE_SKIP=1 turns it off for comparison.
dreamcomp::IdleWait g_wait{.return_pc = 0x0C224800, .flag = 0x8C379E38, .pass_cycles = 22};

}  // namespace

DC_HOOK_ENTRY(idle_wait) {
    g_wait.on_call(c, m);
    return false;  // run the (empty) function
}
