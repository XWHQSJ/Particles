// snow.cpp — Gentle snowfall with low gravity, horizontal drift, ground collision
#include "../common/app.h"
#include "../common/color.h"
#include <GLFW/glfw3.h>

static ParticleSystem sys;
static GLuint tex_id = 0;

int main(int argc, char** argv) {
    CLIArgs cli = parse_cli(argc, argv);
    if (cli.help) { print_help("snow"); return 0; }

    App app;
    AppConfig acfg;
    acfg.title = "Particles — Snow";
    if (!app.init(acfg)) return 1;

    ParticleConfig cfg;
    cfg.count = (cli.particles > 0) ? cli.particles : 2000;
    cfg.slowdown = (cli.slowdown > 0) ? cli.slowdown : 3.0f;
    cfg.gravity = cli.gravity_set
        ? Vec3{cli.gravity_x, cli.gravity_y, cli.gravity_z}
        : Vec3{0.05f, -0.15f, 0.0f};  // light downward + drift
    cfg.spawn_pos = {0.0f, 15.0f, 0.0f};
    cfg.pos_spread = {20.0f, 3.0f, 10.0f};
    cfg.vel_base = {5.0f, -20.0f, 0.0f};
    cfg.vel_spread = {30.0f, 10.0f, 15.0f};
    cfg.respawn_vel_spread = {20.0f, 10.0f, 10.0f};
    cfg.fade_min = 0.002f;
    cfg.fade_range = 0.04f;  // slow fade
    cfg.floor_y = -12.0f;    // ground collision

    app.on_init([&]() {
        tex_id = app.load_texture_from_file("allParticles/Data/Particle.bmp");
        sys.init(cfg, kSnowPalette, kSnowPaletteSize);
    });
    app.on_update([](float dt) { sys.update(dt); });
    app.on_render([](float zoom) { render_particles_legacy(sys, tex_id, zoom); });
    app.on_key([](int key, int action) {
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) sys.burst();
    });
    app.run();
    return 0;
}
