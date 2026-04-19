// explosion.cpp — Radial burst, no gravity, rapid fade
#include "../common/app.h"
#include "../common/color.h"
#include <GLFW/glfw3.h>

static ParticleSystem sys;
static GLuint tex_id = 0;

int main(int argc, char** argv) {
    CLIArgs cli = parse_cli(argc, argv);
    if (cli.help) { print_help("explosion"); return 0; }

    App app;
    AppConfig acfg;
    acfg.title = "Particles — Explosion";
    if (!app.init(acfg)) return 1;

    ParticleConfig cfg;
    cfg.count = (cli.particles > 0) ? cli.particles : 2000;
    cfg.slowdown = (cli.slowdown > 0) ? cli.slowdown : 1.0f;
    cfg.gravity = cli.gravity_set
        ? Vec3{cli.gravity_x, cli.gravity_y, cli.gravity_z}
        : Vec3{0.0f, 0.0f, 0.0f};  // no gravity
    cfg.vel_spread = {400.0f, 400.0f, 400.0f};  // radial burst
    cfg.respawn_vel_spread = {300.0f, 300.0f, 300.0f};
    cfg.fade_min = 0.015f;
    cfg.fade_range = 0.25f;  // rapid fade
    cfg.auto_respawn = true;

    app.on_init([&]() {
        tex_id = app.load_texture_from_file("allParticles/Data/Particle.bmp");
        sys.init(cfg, kExplosionPalette, kExplosionPaletteSize);
    });
    app.on_update([](float dt) { sys.update(dt); });
    app.on_render([](float zoom) { render_particles_legacy(sys, tex_id, zoom); });
    app.on_key([](int key, int action) {
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) sys.burst();
    });
    app.run();
    return 0;
}
