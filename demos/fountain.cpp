// fountain.cpp — Circular spawn via sin/cos, high initial Y, strong negative gravity
#include "../common/app.h"
#include <GLFW/glfw3.h>
#include <cmath>

static constexpr int kStreams = 8;
static ParticleSystem sys[kStreams];
static GLuint tex_id = 0;

int main(int argc, char** argv) {
    CLIArgs cli = parse_cli(argc, argv);
    if (cli.help) { print_help("fountain"); return 0; }

    int n = (cli.particles > 0) ? cli.particles : 500;
    float sd = (cli.slowdown > 0) ? cli.slowdown : 1.8f;

    App app;
    AppConfig acfg;
    acfg.title = "Particles — Fountain";
    if (!app.init(acfg)) return 1;

    app.on_init([&]() {
        tex_id = app.load_texture_from_file("allParticles/Data/Particle.bmp");
        for (int i = 0; i < kStreams; i++) {
            float angle = static_cast<float>(i) / kStreams * 6.2832f;
            float cx = cosf(angle) * 2.0f;
            float cz = sinf(angle) * 2.0f;

            ParticleConfig cfg;
            cfg.count = n;
            cfg.slowdown = sd;
            cfg.gravity = cli.gravity_set
                ? Vec3{cli.gravity_x, cli.gravity_y, cli.gravity_z}
                : Vec3{0.0f, -2.0f, 0.0f};  // strong downward
            cfg.spawn_pos = {cx, -6.0f, cz};
            cfg.pos_spread = {0.5f, 0.0f, 0.5f};
            cfg.vel_base = {cx * 15.0f, 150.0f, cz * 15.0f};  // high upward
            cfg.vel_spread = {20.0f, 30.0f, 20.0f};
            cfg.respawn_vel_spread = {15.0f, 50.0f, 15.0f};
            cfg.fade_min = 0.004f;
            cfg.fade_range = 0.08f;
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
