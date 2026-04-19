// galaxy.cpp — Parametric spiral spawn with tangential velocity
#include "../common/app.h"
#include "../common/color.h"
#include <GLFW/glfw3.h>
#include <cmath>

static constexpr int kArms = 4;
static ParticleSystem sys[kArms];
static GLuint tex_id = 0;

int main(int argc, char** argv) {
    CLIArgs cli = parse_cli(argc, argv);
    if (cli.help) { print_help("galaxy"); return 0; }

    int n = (cli.particles > 0) ? cli.particles : 800;
    float sd = (cli.slowdown > 0) ? cli.slowdown : 2.5f;

    App app;
    AppConfig acfg;
    acfg.title = "Particles — Galaxy";
    if (!app.init(acfg)) return 1;

    app.on_init([&]() {
        tex_id = app.load_texture_from_file("allParticles/Data/Particle.bmp");
        for (int i = 0; i < kArms; i++) {
            float base_angle = static_cast<float>(i) / kArms * 6.2832f;
            // Spiral arm: tangential velocity creates rotation
            float tx = -sinf(base_angle) * 0.5f;
            float ty = cosf(base_angle) * 0.5f;

            ParticleConfig cfg;
            cfg.count = n;
            cfg.slowdown = sd;
            cfg.gravity = cli.gravity_set
                ? Vec3{cli.gravity_x, cli.gravity_y, cli.gravity_z}
                : Vec3{tx, ty, 0.0f};  // tangential pull
            cfg.spawn_pos = {cosf(base_angle) * 3.0f,
                             sinf(base_angle) * 3.0f, 0.0f};
            cfg.pos_spread = {2.0f, 2.0f, 1.0f};
            cfg.vel_base = {-sinf(base_angle) * 50.0f,
                            cosf(base_angle) * 50.0f, 0.0f};
            cfg.vel_spread = {30.0f, 30.0f, 10.0f};
            cfg.respawn_vel_spread = {20.0f, 20.0f, 5.0f};
            cfg.fade_min = 0.003f;
            cfg.fade_range = 0.06f;
            sys[i].init(cfg, kGalaxyPalette, kGalaxyPaletteSize);
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
