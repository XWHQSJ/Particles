#include "particle.h"

static float rand_float(float lo, float hi) {
    return lo + static_cast<float>(rand()) /
                    static_cast<float>(RAND_MAX) * (hi - lo);
}

void ParticleSystem::init(const ParticleConfig& cfg) {
    init(cfg, kRainbowPalette, 12);
}

void ParticleSystem::init(const ParticleConfig& cfg,
                          const Color3* palette, int palette_size) {
    config_ = cfg;
    palette_ = palette;
    palette_size_ = palette_size;
    color_index_ = 0;

    particles_.resize(static_cast<size_t>(cfg.count));
    for (auto& p : particles_) {
        init_particle(p, true);
    }
}

void ParticleSystem::init_particle(Particle& p, bool first_spawn) {
    p.active = true;
    p.life = 1.0f;
    p.fade = config_.fade_min +
             rand_float(0.0f, config_.fade_range);

    int ci = color_index_ % palette_size_;
    p.color = palette_[ci];

    if (first_spawn) {
        // initial wide spread
        p.vel.x = rand_float(-config_.vel_spread.x,
                             config_.vel_spread.x);
        p.vel.y = rand_float(-config_.vel_spread.y,
                             config_.vel_spread.y);
        p.vel.z = rand_float(-config_.vel_spread.z,
                             config_.vel_spread.z);
    } else {
        p.vel.x = config_.vel_base.x +
                  rand_float(-config_.respawn_vel_spread.x,
                             config_.respawn_vel_spread.x);
        p.vel.y = config_.vel_base.y +
                  rand_float(-config_.respawn_vel_spread.y,
                             config_.respawn_vel_spread.y);
        p.vel.z = config_.vel_base.z +
                  rand_float(-config_.respawn_vel_spread.z,
                             config_.respawn_vel_spread.z);
    }

    p.pos.x = config_.spawn_pos.x +
              rand_float(-config_.pos_spread.x, config_.pos_spread.x);
    p.pos.y = config_.spawn_pos.y +
              rand_float(-config_.pos_spread.y, config_.pos_spread.y);
    p.pos.z = config_.spawn_pos.z +
              rand_float(-config_.pos_spread.z, config_.pos_spread.z);

    p.gravity = config_.gravity;
}

void ParticleSystem::respawn_particle(Particle& p) {
    color_index_ = (color_index_ + 1) % palette_size_;
    init_particle(p, false);
}

void ParticleSystem::update(float dt) {
    float sd = config_.slowdown;
    if (sd < 0.01f) sd = 0.01f;
    float time_scale = dt / sd;

    for (auto& p : particles_) {
        if (!p.active) continue;

        // integrate position
        p.pos.x += p.vel.x * time_scale;
        p.pos.y += p.vel.y * time_scale;
        p.pos.z += p.vel.z * time_scale;

        // apply gravity to velocity (gravity is per-frame force)
        p.vel.x += p.gravity.x * dt;
        p.vel.y += p.gravity.y * dt;
        p.vel.z += p.gravity.z * dt;

        // reduce life
        p.life -= p.fade * dt;

        // floor collision
        if (config_.floor_y > -99.0f && p.pos.y < config_.floor_y) {
            p.pos.y = config_.floor_y;
            p.vel.y = -p.vel.y * 0.3f;
        }

        // respawn or deactivate
        if (p.life <= 0.0f) {
            if (config_.auto_respawn) {
                respawn_particle(p);
            } else {
                p.active = false;
            }
        }
    }
}

void ParticleSystem::set_gravity(Vec3 g) {
    config_.gravity = g;
    for (auto& p : particles_) {
        p.gravity = g;
    }
}

void ParticleSystem::set_slowdown(float s) {
    config_.slowdown = s;
}

void ParticleSystem::burst() {
    for (auto& p : particles_) {
        respawn_particle(p);
        p.vel.x = rand_float(-config_.vel_spread.x, config_.vel_spread.x);
        p.vel.y = rand_float(-config_.vel_spread.y, config_.vel_spread.y);
        p.vel.z = rand_float(-config_.vel_spread.z, config_.vel_spread.z);
    }
}
