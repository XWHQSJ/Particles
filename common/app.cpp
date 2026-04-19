#include "app.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

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

bool App::init(const AppConfig& cfg) {
    config_ = cfg;
    g_app = this;

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return false;
    }

    // Request OpenGL 2.1 compatibility for legacy immediate mode
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_SAMPLES, 4);

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
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_TEXTURE_2D);

    return true;
}

GLuint App::load_texture_from_file(const char* path) {
    return load_texture(path);
}

void App::run() {
    if (init_cb_) init_cb_();

    double last_time = glfwGetTime();

    while (!glfwWindowShouldClose(window_)) {
        double now = glfwGetTime();
        float dt = static_cast<float>(now - last_time);
        last_time = now;
        // clamp dt to avoid spiral of death
        if (dt > 0.1f) dt = 0.1f;

        // Handle resize
        int w, h;
        glfwGetFramebufferSize(window_, &w, &h);
        if (h == 0) h = 1;
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, static_cast<double>(w) / h, 0.1, 200.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        if (update_cb_) update_cb_(dt);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        if (render_cb_) render_cb_(config_.zoom);

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}

// ---- Legacy renderer (immediate mode) ----

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
