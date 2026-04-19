#include "app.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLES3/gl3.h>
#endif

#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>

// We include texture.h only in app.cpp so stb_image is compiled once
#include "texture.h"

static App* g_app = nullptr;

void App::key_callback_static(GLFWwindow* w, int key, int scancode,
                              int action, int mods) {
    (void)w; (void)scancode; (void)mods;
    if (g_app && g_app->key_cb_) {
        g_app->key_cb_(key, action);
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(w, GLFW_TRUE);
    }
}

App::~App() {
    if (window_) {
        glfwDestroyWindow(window_);
    }
    glfwTerminate();
    g_app = nullptr;
}

// Build a simple perspective matrix (replaces gluPerspective to avoid GLU dep)
static void build_perspective(float* m, float fov_deg, float aspect,
                              float near_p, float far_p) {
    memset(m, 0, 16 * sizeof(float));
    float f = 1.0f / tanf(fov_deg * 3.14159265f / 360.0f);
    m[0] = f / aspect;
    m[5] = f;
    m[10] = (far_p + near_p) / (near_p - far_p);
    m[11] = -1.0f;
    m[14] = (2.0f * far_p * near_p) / (near_p - far_p);
}

bool App::init(const AppConfig& cfg) {
    config_ = cfg;
    g_app = this;

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return false;
    }

#ifdef __EMSCRIPTEN__
    // WebGL 2 (maps to OpenGL ES 3.0)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#else
    // Request OpenGL 2.1 compatibility for legacy immediate mode
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_SAMPLES, 4);
#endif

    window_ = glfwCreateWindow(cfg.width, cfg.height,
                               cfg.title.c_str(), nullptr, nullptr);
    if (!window_) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window_, key_callback_static);

    // GL setup
#ifndef __EMSCRIPTEN__
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_TEXTURE_2D);
#endif
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    return true;
}

GLuint App::load_texture_from_file(const char* path) {
    return load_texture(path);
}

// Single-frame tick for both Emscripten and desktop run()
void App::tick() {
    double now = glfwGetTime();
    float dt = static_cast<float>(now - last_time_);
    last_time_ = now;
    if (dt > 0.1f) dt = 0.1f;

    int w, h;
    glfwGetFramebufferSize(window_, &w, &h);
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);

#ifndef __EMSCRIPTEN__
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    {
        float proj[16];
        build_perspective(proj, 45.0f, static_cast<float>(w) / h, 0.1f, 200.0f);
        glLoadMatrixf(proj);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#endif

    if (update_cb_) update_cb_(dt);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifndef __EMSCRIPTEN__
    glLoadIdentity();
#endif

    if (render_cb_) render_cb_(config_.zoom);

    glfwSwapBuffers(window_);
    glfwPollEvents();
}

#ifdef __EMSCRIPTEN__
static void emscripten_main_loop(void* arg) {
    auto* app = static_cast<App*>(arg);
    app->tick();
}
#endif

void App::run() {
    if (init_cb_) init_cb_();

    last_time_ = glfwGetTime();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(emscripten_main_loop, this, 0, 1);
#else
    while (!glfwWindowShouldClose(window_)) {
        tick();
    }
#endif
}

// ---- Legacy renderer (immediate mode) ----
// Not available under Emscripten — demos should use the modern renderer
// for WASM builds. This stub prevents link errors.

#ifdef __EMSCRIPTEN__
#include "renderer_modern.h"

static ModernRenderer g_web_renderer;
static bool g_web_renderer_inited = false;

void render_particles_legacy(const ParticleSystem& sys,
                             GLuint texture_id, float zoom) {
    // On WebGL, transparently delegate to the modern renderer
    if (!g_web_renderer_inited) {
        g_web_renderer.init(nullptr, nullptr);
        g_web_renderer_inited = true;
    }
    if (g_web_renderer.is_ready()) {
        // Use a default viewport size; the real viewport is set in tick()
        int vp[4];
        glGetIntegerv(GL_VIEWPORT, vp);
        g_web_renderer.render(sys, texture_id, zoom, vp[2], vp[3]);
    }
}
#else
void render_particles_legacy(const ParticleSystem& sys,
                             GLuint texture_id, float zoom) {
    glBindTexture(GL_TEXTURE_2D, texture_id);
    const auto& parts = sys.particles();
    for (const auto& p : parts) {
        if (!p.active) continue;
        float x = p.pos.x;
        float y = p.pos.y;
        float z = p.pos.z + zoom;

        glColor4f(p.color.r, p.color.g, p.color.b, p.life);
        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(1, 1); glVertex3f(x + 0.5f, y + 0.5f, z);
        glTexCoord2f(0, 1); glVertex3f(x - 0.5f, y + 0.5f, z);
        glTexCoord2f(1, 0); glVertex3f(x + 0.5f, y - 0.5f, z);
        glTexCoord2f(0, 0); glVertex3f(x - 0.5f, y - 0.5f, z);
        glEnd();
    }
}
#endif

// ---- CLI parsing ----

CLIArgs parse_cli(int argc, char** argv) {
    CLIArgs args;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            args.help = true;
        } else if (strcmp(argv[i], "--particles") == 0 && i + 1 < argc) {
            args.particles = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--gravity") == 0 && i + 1 < argc) {
            args.gravity_set = true;
            char* s = argv[++i];
            args.gravity_x = static_cast<float>(strtod(s, &s));
            if (*s == ',') s++;
            args.gravity_y = static_cast<float>(strtod(s, &s));
            if (*s == ',') s++;
            args.gravity_z = static_cast<float>(strtod(s, &s));
        } else if (strcmp(argv[i], "--slowdown") == 0 && i + 1 < argc) {
            args.slowdown = static_cast<float>(atof(argv[++i]));
        }
    }
    return args;
}

void print_help(const char* demo_name) {
    printf("Usage: %s [OPTIONS]\n\n", demo_name);
    printf("Options:\n");
    printf("  --particles N       Number of particles (default: demo-specific)\n");
    printf("  --gravity x,y,z     Gravity vector (e.g. 0,-0.8,0)\n");
    printf("  --slowdown F        Slowdown factor (default: 2.0)\n");
    printf("  --help, -h          Show this help\n");
    printf("\nControls:\n");
    printf("  ESC                 Quit\n");
    printf("  SPACE               Burst (reset all particles)\n");
}
