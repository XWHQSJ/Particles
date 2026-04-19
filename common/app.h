#pragma once

#include <string>
#include <vector>
#include <functional>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#elif defined(__APPLE__)
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "particle.h"

struct GLFW_Window; // forward declare, we use void* internally

struct AppConfig {
    std::string title = "Particles";
    int width = 800;
    int height = 600;
    bool use_modern_gl = false;
    float zoom = -40.0f;
};

// Callback types for demo logic
using InitCallback = std::function<void()>;
using UpdateCallback = std::function<void(float dt)>;
using RenderCallback = std::function<void(float zoom)>;
using KeyCallback = std::function<void(int key, int action)>;

class App {
public:
    App() = default;
    ~App();

    bool init(const AppConfig& cfg);
    void run();

    // Register callbacks
    void on_init(InitCallback cb) { init_cb_ = cb; }
    void on_update(UpdateCallback cb) { update_cb_ = cb; }
    void on_render(RenderCallback cb) { render_cb_ = cb; }
    void on_key(KeyCallback cb) { key_cb_ = cb; }

    GLuint load_texture_from_file(const char* path);

    // Called once per frame; public for emscripten_set_main_loop_arg
    void tick();

private:
    struct GLFWwindow* window_ = nullptr;
    AppConfig config_;
    double last_time_ = 0.0;

    InitCallback init_cb_;
    UpdateCallback update_cb_;
    RenderCallback render_cb_;
    KeyCallback key_cb_;

    static void key_callback_static(struct GLFWwindow* w,
                                    int key, int scancode,
                                    int action, int mods);
};

// Parse common CLI flags. Returns remaining args.
struct CLIArgs {
    int particles = -1;   // -1 = use demo default
    float gravity_x = 0.0f;
    float gravity_y = 0.0f;
    float gravity_z = 0.0f;
    bool gravity_set = false;
    float slowdown = -1.0f; // -1 = use demo default
    bool help = false;
};

CLIArgs parse_cli(int argc, char** argv);
void print_help(const char* demo_name);

// Legacy immediate-mode renderer (OpenGL 2.1)
void render_particles_legacy(const ParticleSystem& sys,
                             GLuint texture_id, float zoom);
