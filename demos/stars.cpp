// stars.cpp — 7 preset particle systems at different positions
// Port of allParticles/stars.cpp which had 7 separate arrays
#include "../common/app.h"
#include <GLFW/glfw3.h>

static constexpr int kNumSystems = 7;
static ParticleSystem sys[kNumSystems];
static GLuint tex_id = 0;

int main(int argc, char** argv) {
    CLIArgs cli = parse_cli(argc, argv);
    if (cli.help) { print_help("stars"); return 0; }

    int n = (cli.particles > 0) ? cli.particles : 500;
    float sd = (cli.slowdown > 0) ? cli.slowdown : 2.0f;

    // 7 systems with different gravity directions (from original)
    struct Preset { Vec3 gravity; };
    Preset presets[kNumSystems] = {
        {{ 3.0f,  2.0f, 0.0f}},
        {{ 3.0f,  2.0f, 0.0f}},
        {{-3.0f, -2.0f, 0.0f}},
        {{ 0.0f,  3.0f, 0.0f}},
        {{ 0.0f, -3.0f, 0.0f}},
        {{ 2.0f,  0.0f, 0.0f}},
        {{-2.0f,  0.0f, 0.0f}},
    };

    App app;
    AppConfig acfg;
    acfg.title = "Particles — Stars";
    if (!app.init(acfg)) return 1;

    app.on_init([&]() {
        tex_id = app.load_texture_from_file("allParticles/Data/Particle.bmp");
        for (int i = 0; i < kNumSystems; i++) {
            ParticleConfig cfg;
            cfg.count = n;
            cfg.slowdown = sd;
            cfg.gravity = cli.gravity_set
                ? Vec3{cli.gravity_x, cli.gravity_y, cli.gravity_z}
                : presets[i].gravity;
            cfg.vel_spread = {260.0f, 250.0f, 250.0f};
            cfg.respawn_vel_spread = {32.0f, 30.0f, 30.0f};
            sys[i].init(cfg);
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
