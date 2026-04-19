// particles.cpp — Base NeHe-style particle demo (port of allParticles/particles.cpp)
#include "../common/app.h"
#include <GLFW/glfw3.h>

static ParticleSystem sys;
static GLuint tex_id = 0;

int main(int argc, char** argv) {
    CLIArgs cli = parse_cli(argc, argv);
    if (cli.help) { print_help("particles"); return 0; }

    App app;
    AppConfig acfg;
    acfg.title = "Particles — Base Demo";

    ParticleConfig cfg;
    cfg.count = (cli.particles > 0) ? cli.particles : 1000;
    cfg.gravity = cli.gravity_set ? Vec3{cli.gravity_x, cli.gravity_y, cli.gravity_z}
                                  : Vec3{0.0f, -0.8f, 0.0f};
    cfg.slowdown = (cli.slowdown > 0) ? cli.slowdown : 2.0f;
    cfg.vel_spread = {260.0f, 250.0f, 250.0f};
    cfg.respawn_vel_spread = {32.0f, 30.0f, 30.0f};

    if (!app.init(acfg)) return 1;

    app.on_init([&]() {
        tex_id = app.load_texture_from_file("allParticles/Data/Particle.bmp");
        sys.init(cfg);
    });
    app.on_update([](float dt) { sys.update(dt); });
    app.on_render([](float zoom) { render_particles_legacy(sys, tex_id, zoom); });
    app.on_key([](int key, int action) {
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) sys.burst();
    });
    app.run();
    return 0;
}
