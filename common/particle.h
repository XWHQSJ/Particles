#pragma once

#include <cmath>
#include <cstdlib>
#include <vector>

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct Color3 {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
};

struct Particle {
    bool active = true;
    float life = 1.0f;
    float fade = 0.01f;
    Color3 color;
    Vec3 pos;
    Vec3 vel;
    Vec3 gravity;
};

struct ParticleConfig {
    int count = 1000;
    float slowdown = 2.0f;
    float zoom = -40.0f;

    Vec3 spawn_pos = {0.0f, 0.0f, 0.0f};
    Vec3 gravity = {0.0f, -0.8f, 0.0f};

    // velocity spread: vel = base + rand(-spread, spread)
    Vec3 vel_base = {0.0f, 0.0f, 0.0f};
    Vec3 vel_spread = {260.0f, 250.0f, 250.0f};

    float fade_min = 0.003f;
    float fade_range = 0.1f; // fade = fade_min + rand(0, fade_range)

    // position spread on respawn
    Vec3 pos_spread = {0.0f, 0.0f, 0.0f};

    // respawn velocity spread
    Vec3 respawn_vel_spread = {32.0f, 30.0f, 30.0f};

    bool auto_respawn = true;

    // collision floor (negative = disabled)
    float floor_y = -100.0f;
};

// 12-color rainbow palette from original NeHe code
static const Color3 kRainbowPalette[12] = {
    {1.0f, 0.5f, 0.5f},  {1.0f, 0.75f, 0.5f}, {1.0f, 1.0f, 0.5f},
    {0.75f, 1.0f, 0.5f},  {0.5f, 1.0f, 0.5f},  {0.5f, 1.0f, 0.75f},
    {0.5f, 1.0f, 1.0f},   {0.5f, 0.75f, 1.0f},  {0.5f, 0.5f, 1.0f},
    {0.75f, 0.5f, 1.0f},  {1.0f, 0.5f, 1.0f},   {1.0f, 0.5f, 0.75f},
};

class ParticleSystem {
public:
    ParticleSystem() = default;

    void init(const ParticleConfig& cfg);
    void init(const ParticleConfig& cfg,
              const Color3* palette, int palette_size);
    void update(float dt);

    // Access for rendering
    const std::vector<Particle>& particles() const { return particles_; }
    const ParticleConfig& config() const { return config_; }

    // Runtime adjustments
    void set_gravity(Vec3 g);
    void set_slowdown(float s);
    void burst(); // reset all particles to origin

private:
    void init_particle(Particle& p, bool first_spawn);
    void respawn_particle(Particle& p);

    ParticleConfig config_;
    std::vector<Particle> particles_;
    const Color3* palette_ = kRainbowPalette;
    int palette_size_ = 12;
    int color_index_ = 0;
};
