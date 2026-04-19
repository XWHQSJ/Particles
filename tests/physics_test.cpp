// physics_test.cpp — Unit tests for ParticleSystem physics (no GL context needed)
#include <gtest/gtest.h>
#include "particle.h"
#include <cmath>

class ParticleSystemTest : public ::testing::Test {
protected:
    ParticleSystem sys;
    ParticleConfig default_cfg() {
        ParticleConfig cfg;
        cfg.count = 100;
        cfg.slowdown = 1.0f;
        cfg.gravity = {0.0f, -1.0f, 0.0f};
        cfg.vel_spread = {50.0f, 50.0f, 50.0f};
        cfg.respawn_vel_spread = {30.0f, 30.0f, 30.0f};
        cfg.fade_min = 0.01f;
        cfg.fade_range = 0.05f;
        cfg.auto_respawn = false; // disable respawn for deterministic tests
        return cfg;
    }
};

// Life decreases monotonically with update
TEST_F(ParticleSystemTest, LifeDecreasesMonotonically) {
    auto cfg = default_cfg();
    sys.init(cfg);

    float initial_life = sys.particles()[0].life;
    EXPECT_FLOAT_EQ(initial_life, 1.0f);

    sys.update(1.0f);

    for (const auto& p : sys.particles()) {
        // Life should be strictly less than initial (1.0)
        // unless particle became inactive
        if (p.active) {
            EXPECT_LT(p.life, initial_life);
        }
    }
}

// Multiple updates keep decreasing life
TEST_F(ParticleSystemTest, LifeDecreasesOverMultipleSteps) {
    auto cfg = default_cfg();
    cfg.fade_min = 0.1f;
    cfg.fade_range = 0.0f; // fixed fade rate
    sys.init(cfg);

    float prev_life = sys.particles()[0].life;
    for (int step = 0; step < 5; step++) {
        sys.update(1.0f);
        float cur_life = sys.particles()[0].life;
        if (sys.particles()[0].active) {
            EXPECT_LT(cur_life, prev_life)
                << "Life should decrease at step " << step;
        }
        prev_life = cur_life;
    }
}

// Velocity integration: position changes by vel * dt / slowdown
TEST_F(ParticleSystemTest, VelocityIntegration) {
    auto cfg = default_cfg();
    cfg.count = 1;
    cfg.gravity = {0.0f, 0.0f, 0.0f}; // no gravity
    cfg.slowdown = 1.0f;
    cfg.vel_spread = {0.0f, 0.0f, 0.0f}; // zero initial spread
    cfg.vel_base = {0.0f, 0.0f, 0.0f};
    sys.init(cfg);

    // Manually check: with zero velocity, position shouldn't change
    float x0 = sys.particles()[0].pos.x;
    float y0 = sys.particles()[0].pos.y;
    sys.update(1.0f);
    // Position change should be approximately zero (velocity was random but spread is 0)
    EXPECT_NEAR(sys.particles()[0].pos.x, x0, 0.01f);
    EXPECT_NEAR(sys.particles()[0].pos.y, y0, 0.01f);
}

// Gravity accumulates into velocity
TEST_F(ParticleSystemTest, GravityAccumulation) {
    auto cfg = default_cfg();
    cfg.count = 1;
    cfg.gravity = {0.0f, -2.0f, 0.0f};
    cfg.slowdown = 1.0f;
    cfg.vel_spread = {0.0f, 0.0f, 0.0f};
    cfg.vel_base = {0.0f, 0.0f, 0.0f};
    sys.init(cfg);

    float vy_before = sys.particles()[0].vel.y;
    sys.update(1.0f);
    float vy_after = sys.particles()[0].vel.y;

    // Velocity should have decreased by gravity * dt = -2.0 * 1.0 = -2.0
    EXPECT_NEAR(vy_after - vy_before, -2.0f, 0.01f);
}

// Gravity in X direction
TEST_F(ParticleSystemTest, GravityAccumulationX) {
    auto cfg = default_cfg();
    cfg.count = 1;
    cfg.gravity = {3.0f, 0.0f, 0.0f};
    cfg.slowdown = 1.0f;
    cfg.vel_spread = {0.0f, 0.0f, 0.0f};
    cfg.vel_base = {0.0f, 0.0f, 0.0f};
    sys.init(cfg);

    float vx_before = sys.particles()[0].vel.x;
    sys.update(0.5f);
    float vx_after = sys.particles()[0].vel.x;

    // vel += gravity * dt = 3.0 * 0.5 = 1.5
    EXPECT_NEAR(vx_after - vx_before, 1.5f, 0.01f);
}

// Lifetime expiry: particle becomes inactive when life <= 0
TEST_F(ParticleSystemTest, LifetimeExpiry) {
    auto cfg = default_cfg();
    cfg.count = 10;
    cfg.fade_min = 0.5f;
    cfg.fade_range = 0.0f;
    cfg.auto_respawn = false;
    sys.init(cfg);

    // After enough updates, all particles should be inactive
    for (int i = 0; i < 10; i++) {
        sys.update(1.0f);
    }

    int active_count = 0;
    for (const auto& p : sys.particles()) {
        if (p.active) active_count++;
    }
    EXPECT_EQ(active_count, 0) << "All particles should expire with fast fade";
}

// Auto-respawn: particles get new life when they expire
TEST_F(ParticleSystemTest, AutoRespawn) {
    auto cfg = default_cfg();
    cfg.count = 50;
    cfg.fade_min = 1.0f;  // very fast fade
    cfg.fade_range = 0.0f;
    cfg.auto_respawn = true;
    sys.init(cfg);

    // After update, expired particles should be respawned
    sys.update(2.0f); // enough to expire all

    int active_count = 0;
    for (const auto& p : sys.particles()) {
        if (p.active) active_count++;
    }
    EXPECT_EQ(active_count, 50) << "All particles should be respawned";
}

// Burst resets all particles
TEST_F(ParticleSystemTest, BurstResetsParticles) {
    auto cfg = default_cfg();
    cfg.count = 50;
    cfg.auto_respawn = true;
    sys.init(cfg);

    sys.update(1.0f); // move particles

    sys.burst();

    // After burst, all should be active with full life
    for (const auto& p : sys.particles()) {
        EXPECT_TRUE(p.active);
        EXPECT_FLOAT_EQ(p.life, 1.0f);
    }
}

// Floor collision: particle bounces at floor_y
TEST_F(ParticleSystemTest, FloorCollision) {
    auto cfg = default_cfg();
    cfg.count = 1;
    cfg.gravity = {0.0f, -10.0f, 0.0f};
    cfg.slowdown = 1.0f;
    cfg.vel_spread = {0.0f, 0.0f, 0.0f};
    cfg.vel_base = {0.0f, -100.0f, 0.0f};
    cfg.floor_y = -5.0f;
    cfg.auto_respawn = false;
    sys.init(cfg);

    // Run several updates
    for (int i = 0; i < 10; i++) {
        sys.update(0.1f);
    }

    // Particle y should not go below floor
    if (sys.particles()[0].active) {
        EXPECT_GE(sys.particles()[0].pos.y, cfg.floor_y);
    }
}

// Slowdown affects position integration
TEST_F(ParticleSystemTest, SlowdownEffect) {
    // Create two systems with different slowdown values
    // Use no gravity, long life, known initial velocity
    auto cfg1 = default_cfg();
    cfg1.count = 1;
    cfg1.slowdown = 1.0f;
    cfg1.gravity = {0.0f, 0.0f, 0.0f};
    cfg1.vel_spread = {0.0f, 0.0f, 0.0f}; // zero spread = zero initial vel
    cfg1.auto_respawn = false;
    cfg1.fade_min = 0.001f;
    cfg1.fade_range = 0.0f;

    auto cfg2 = cfg1;
    cfg2.slowdown = 2.0f;

    ParticleSystem s1, s2;
    s1.init(cfg1);
    s2.init(cfg2);

    // After init, velocity is zero (spread=0 in first_spawn path).
    // Apply gravity to give them known velocity, then measure position delta.
    s1.set_gravity({100.0f, 0.0f, 0.0f});
    s2.set_gravity({100.0f, 0.0f, 0.0f});

    // First update: gravity adds velocity = 100 * 0.1 = 10 to both
    // Position moves by vel * dt / slowdown
    s1.update(0.1f);
    s2.update(0.1f);
    float x1a = s1.particles()[0].pos.x;
    float x2a = s2.particles()[0].pos.x;

    // Second update: now vel ~= 10 + 100*0.1 = 20 for both
    // pos1 += 20 * 0.1 / 1.0 = 2.0
    // pos2 += 20 * 0.1 / 2.0 = 1.0
    s1.update(0.1f);
    s2.update(0.1f);
    float dx1 = std::abs(s1.particles()[0].pos.x - x1a);
    float dx2 = std::abs(s2.particles()[0].pos.x - x2a);

    EXPECT_GT(dx1, dx2) << "Lower slowdown should mean faster movement";
}

// Set gravity at runtime
TEST_F(ParticleSystemTest, SetGravityRuntime) {
    auto cfg = default_cfg();
    cfg.count = 1;
    cfg.gravity = {0.0f, 0.0f, 0.0f};
    cfg.vel_spread = {0.0f, 0.0f, 0.0f};
    cfg.slowdown = 1.0f;
    sys.init(cfg);

    sys.set_gravity({0.0f, -5.0f, 0.0f});
    sys.update(1.0f);

    float vy = sys.particles()[0].vel.y;
    EXPECT_NEAR(vy, -5.0f, 0.01f);
}

// Palette colors are applied
TEST_F(ParticleSystemTest, PaletteColorsApplied) {
    static const Color3 custom_palette[] = {
        {0.1f, 0.2f, 0.3f},
    };
    auto cfg = default_cfg();
    cfg.count = 10;
    sys.init(cfg, custom_palette, 1);

    for (const auto& p : sys.particles()) {
        EXPECT_FLOAT_EQ(p.color.r, 0.1f);
        EXPECT_FLOAT_EQ(p.color.g, 0.2f);
        EXPECT_FLOAT_EQ(p.color.b, 0.3f);
    }
}
