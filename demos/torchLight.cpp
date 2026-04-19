// torchLight.cpp — Flame / torch upward effect
// Port of allParticles/torchLight.cpp — multiple systems with upward gravity
#include "../common/app.h"
#include "../common/color.h"
#include <GLFW/glfw3.h>

static constexpr int kNumFlames = 4;
static ParticleSystem sys[kNumFlames];
static GLuint tex_id = 0;

int main(int argc, char** argv) {
    CLIArgs cli = parse_cli(argc, argv);
    if (cli.help) { print_help("torchLight"); return 0; }

    int n = (cli.particles > 0) ? cli.particles : 800;
    float sd = (cli.slowdown > 0) ? cli.slowdown : 2.0f;

    // Original had multiple systems with upward pull
    struct Preset { Vec3 gravity; Vec3 spawn; };
    Preset presets[kNumFlames] = {
        {{ 0.87f, 1.5f, 0.0f}, {0.0f, -5.0f, 0.0f}},
        {{ 0.0f,  2.0f, 0.0f}, {0.0f, -5.0f, 0.0f}},
        {{-0.87f, 1.5f, 0.0f}, {0.0f, -5.0f, 0.0f}},
        {{ 0.0f,  1.0f, 0.0f}, {0.0f, -5.0f, 0.0f}},
    };

    App app;
    AppConfig acfg;
    acfg.title = "Particles — Torch Light";
    if (!app.init(acfg)) return 1;

    app.on_init([&]() {
        tex_id = app.load_texture_from_file("allParticles/Data/Particle.bmp");
        for (int i = 0; i < kNumFlames; i++) {
            ParticleConfig cfg;
            cfg.count = n;
            cfg.slowdown = sd;
            cfg.gravity = cli.gravity_set
                ? Vec3{cli.gravity_x, cli.gravity_y, cli.gravity_z}
                : presets[i].gravity;
            cfg.spawn_pos = presets[i].spawn;
            cfg.vel_spread = {200.0f, 200.0f, 200.0f};
            cfg.respawn_vel_spread = {25.0f, 25.0f, 25.0f};
            cfg.fade_min = 0.005f;
            cfg.fade_range = 0.15f;
            sys[i].init(cfg, kFirePalette, kFirePaletteSize);
        }
    });
    app.on_update([](float dt) {
        for (auto& s : sys) s.update(dt);
    });
    app.on_render([](float zoom) {
        for (auto& s : sys) render_particles_legacy(s, tex_id, zoom);
    });
    app.on_key([](int key, int action) {
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
            for (auto& s : sys) s.burst();
    });
    app.run();
    return 0;
}
