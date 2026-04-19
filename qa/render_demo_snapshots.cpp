// render_demo_snapshots.cpp -- Software-rendered particle demo snapshots
// No GL context required. Uses ParticleSystem from common/ for physics,
// then splats particles onto an RGBA buffer and writes PNG via stb.

#include "../common/particle.h"
#include "../common/color.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static constexpr int kWidth = 600;
static constexpr int kHeight = 600;

struct RGBA {
    uint8_t r, g, b, a;
};

// Additive-blend a colored dot at (cx, cy) with radius and alpha
static void splat(RGBA* buf, int w, int h,
                  float cx, float cy, float radius,
                  float cr, float cg, float cb, float alpha) {
    int x0 = std::max(0, static_cast<int>(cx - radius));
    int y0 = std::max(0, static_cast<int>(cy - radius));
    int x1 = std::min(w - 1, static_cast<int>(cx + radius));
    int y1 = std::min(h - 1, static_cast<int>(cy + radius));
    float r2 = radius * radius;

    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            float dx = static_cast<float>(x) - cx;
            float dy = static_cast<float>(y) - cy;
            float d2 = dx * dx + dy * dy;
            if (d2 > r2) continue;

            // Gaussian-ish falloff
            float t = 1.0f - d2 / r2;
            t = t * t;
            float a = alpha * t;

            RGBA& px = buf[y * w + x];
            // Additive blend (clamped)
            int nr = static_cast<int>(px.r) + static_cast<int>(cr * 255.0f * a);
            int ng = static_cast<int>(px.g) + static_cast<int>(cg * 255.0f * a);
            int nb = static_cast<int>(px.b) + static_cast<int>(cb * 255.0f * a);
            px.r = static_cast<uint8_t>(std::min(nr, 255));
            px.g = static_cast<uint8_t>(std::min(ng, 255));
            px.b = static_cast<uint8_t>(std::min(nb, 255));
            px.a = 255;
        }
    }
}

// Project a 3D particle position to 2D screen coords
// Uses a simple perspective: screen = (pos.xy / (zoom_dist - pos.z)) * scale + center
static void project(const Vec3& pos, float zoom,
                    float& sx, float& sy) {
    float depth = -zoom + pos.z;
    if (depth < 1.0f) depth = 1.0f;
    float scale = 500.0f / depth;
    sx = kWidth / 2.0f + pos.x * scale;
    sy = kHeight / 2.0f - pos.y * scale; // Y flipped for screen coords
}

struct DemoSpec {
    const char* name;
    // Returns number of particle systems configured
    int (*setup)(ParticleSystem* sys, int max_sys);
    float zoom;
    int sim_steps;
    float dt;
    float particle_radius;
};

// --- Demo setup functions ---

static int setup_fire(ParticleSystem* sys, int) {
    ParticleConfig cfg;
    cfg.count = 1500;
    cfg.slowdown = 1.5f;
    cfg.gravity = {0.0f, 0.6f, 0.0f};
    cfg.spawn_pos = {0.0f, -8.0f, 0.0f};
    cfg.pos_spread = {3.0f, 0.5f, 1.0f};
    cfg.vel_base = {0.0f, 80.0f, 0.0f};
    cfg.vel_spread = {40.0f, 30.0f, 20.0f};
    cfg.respawn_vel_spread = {20.0f, 40.0f, 10.0f};
    cfg.fade_min = 0.01f;
    cfg.fade_range = 0.2f;
    sys[0].init(cfg, kFirePalette, kFirePaletteSize);
    return 1;
}

static int setup_fountain(ParticleSystem* sys, int) {
    constexpr int kStreams = 8;
    for (int i = 0; i < kStreams; i++) {
        float angle = static_cast<float>(i) / kStreams * 6.2832f;
        float cx = cosf(angle) * 2.0f;
        float cz = sinf(angle) * 2.0f;
        ParticleConfig cfg;
        cfg.count = 500;
        cfg.slowdown = 1.8f;
        cfg.gravity = {0.0f, -2.0f, 0.0f};
        cfg.spawn_pos = {cx, -6.0f, cz};
        cfg.pos_spread = {0.5f, 0.0f, 0.5f};
        cfg.vel_base = {cx * 15.0f, 150.0f, cz * 15.0f};
        cfg.vel_spread = {20.0f, 30.0f, 20.0f};
        cfg.respawn_vel_spread = {15.0f, 50.0f, 15.0f};
        cfg.fade_min = 0.004f;
        cfg.fade_range = 0.08f;
        sys[i].init(cfg);
    }
    return kStreams;
}

static int setup_snow(ParticleSystem* sys, int) {
    ParticleConfig cfg;
    cfg.count = 2000;
    cfg.slowdown = 3.0f;
    cfg.gravity = {0.05f, -0.15f, 0.0f};
    cfg.spawn_pos = {0.0f, 15.0f, 0.0f};
    cfg.pos_spread = {20.0f, 3.0f, 10.0f};
    cfg.vel_base = {5.0f, -20.0f, 0.0f};
    cfg.vel_spread = {30.0f, 10.0f, 15.0f};
    cfg.respawn_vel_spread = {20.0f, 10.0f, 10.0f};
    cfg.fade_min = 0.002f;
    cfg.fade_range = 0.04f;
    cfg.floor_y = -12.0f;
    sys[0].init(cfg, kSnowPalette, kSnowPaletteSize);
    return 1;
}

static int setup_galaxy(ParticleSystem* sys, int) {
    constexpr int kArms = 4;
    for (int i = 0; i < kArms; i++) {
        float base_angle = static_cast<float>(i) / kArms * 6.2832f;
        float tx = -sinf(base_angle) * 0.5f;
        float ty = cosf(base_angle) * 0.5f;
        ParticleConfig cfg;
        cfg.count = 800;
        cfg.slowdown = 2.5f;
        cfg.gravity = {tx, ty, 0.0f};
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
    return kArms;
}

static int setup_explosion(ParticleSystem* sys, int) {
    ParticleConfig cfg;
    cfg.count = 2000;
    cfg.slowdown = 1.0f;
    cfg.gravity = {0.0f, 0.0f, 0.0f};
    cfg.vel_spread = {400.0f, 400.0f, 400.0f};
    cfg.respawn_vel_spread = {300.0f, 300.0f, 300.0f};
    cfg.fade_min = 0.015f;
    cfg.fade_range = 0.25f;
    cfg.auto_respawn = true;
    sys[0].init(cfg, kExplosionPalette, kExplosionPaletteSize);
    return 1;
}

static const DemoSpec kDemos[] = {
    {"fire",      setup_fire,      -40.0f,  80, 0.016f, 6.0f},
    {"fountain",  setup_fountain,  -40.0f,  60, 0.016f, 5.0f},
    {"snow",      setup_snow,      -40.0f, 120, 0.016f, 4.5f},
    {"galaxy",    setup_galaxy,    -40.0f,  80, 0.016f, 5.0f},
    {"explosion", setup_explosion, -40.0f,  20, 0.016f, 5.5f},
};

int main(int argc, char** argv) {
    std::string out_dir = ".";
    if (argc > 1) out_dir = argv[1];

    srand(42); // deterministic snapshots

    for (const auto& demo : kDemos) {
        printf("Rendering %s...\n", demo.name);

        ParticleSystem sys[8]; // max 8 sub-systems
        int nsys = demo.setup(sys, 8);

        // Simulate for N steps to let particles spread
        for (int step = 0; step < demo.sim_steps; step++) {
            for (int s = 0; s < nsys; s++) {
                sys[s].update(demo.dt);
            }
        }

        // Allocate framebuffer -- dark background
        auto* buf = new RGBA[kWidth * kHeight];
        for (int i = 0; i < kWidth * kHeight; i++) {
            buf[i] = {8, 8, 12, 255};
        }

        // Render all particles
        for (int s = 0; s < nsys; s++) {
            for (const auto& p : sys[s].particles()) {
                if (!p.active || p.life <= 0.0f) continue;

                float sx, sy;
                project(p.pos, demo.zoom, sx, sy);

                // Skip off-screen
                if (sx < -20 || sx > kWidth + 20 ||
                    sy < -20 || sy > kHeight + 20) continue;

                float alpha = std::min(p.life * 1.5f, 1.0f);
                splat(buf, kWidth, kHeight,
                      sx, sy, demo.particle_radius,
                      p.color.r, p.color.g, p.color.b, alpha);
            }
        }

        // Write PNG
        std::string path = out_dir + "/" + demo.name + ".png";
        int ok = stbi_write_png(path.c_str(), kWidth, kHeight, 4,
                                buf, kWidth * 4);
        if (ok) {
            printf("  -> %s\n", path.c_str());
        } else {
            fprintf(stderr, "  FAILED to write %s\n", path.c_str());
        }
        delete[] buf;
    }

    printf("Done.\n");
    return 0;
}
