// starCross.cpp — 2 perpendicular particle streams (port of allParticles/starCross.cpp)
// Original: vertical stream + horizontal stream crossing at center
#include "../common/app.h"
#include <GLFW/glfw3.h>

static ParticleSystem center_sys;
static ParticleSystem stream_v;  // vertical
static ParticleSystem stream_h;  // horizontal

static GLuint tex_id = 0;

int main(int argc, char** argv) {
    CLIArgs cli = parse_cli(argc, argv);
    if (cli.help) { print_help("starCross"); return 0; }

    int n = (cli.particles > 0) ? cli.particles : 1000;
    float sd = (cli.slowdown > 0) ? cli.slowdown : 2.0f;

    App app;
    AppConfig acfg;
    acfg.title = "Particles — Star Cross";
    if (!app.init(acfg)) return 1;

    app.on_init([&]() {
        tex_id = app.load_texture_from_file("allParticles/Data/Particle.bmp");

        // Center system (like original base)
        ParticleConfig c0;
        c0.count = n; c0.slowdown = sd;
        c0.gravity = {0.0f, -0.8f, 0.0f};
        center_sys.init(c0);

        // Vertical streams (gravity pulls down)
        ParticleConfig cv;
        cv.count = n; cv.slowdown = sd;
        cv.gravity = cli.gravity_set
            ? Vec3{cli.gravity_x, cli.gravity_y, cli.gravity_z}
            : Vec3{0.0f, -0.1f, 0.0f};
        cv.vel_spread = {260.0f, 250.0f, 250.0f};
        cv.respawn_vel_spread = {32.0f, 30.0f, 30.0f};
        stream_v.init(cv);

        // Horizontal streams (gravity pulls left)
        ParticleConfig ch;
        ch.count = n; ch.slowdown = sd;
        ch.gravity = cli.gravity_set
            ? Vec3{cli.gravity_x, cli.gravity_y, cli.gravity_z}
            : Vec3{-0.1f, 0.0f, 0.0f};
        ch.vel_spread = {260.0f, 250.0f, 250.0f};
        ch.respawn_vel_spread = {32.0f, 30.0f, 30.0f};
        stream_h.init(ch);
    });
    app.on_update([](float dt) {
        center_sys.update(dt); stream_v.update(dt); stream_h.update(dt);
    });
    app.on_render([](float zoom) {
        render_particles_legacy(center_sys, tex_id, zoom);
        render_particles_legacy(stream_v, tex_id, zoom);
        render_particles_legacy(stream_h, tex_id, zoom);
    });
    app.on_key([](int key, int action) {
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
            center_sys.burst(); stream_v.burst(); stream_h.burst();
        }
    });
    app.run();
    return 0;
}
