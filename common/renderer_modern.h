#pragma once

#ifdef __APPLE__
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

#include "particle.h"

// Modern OpenGL 3.3 core renderer using VBOs and shaders.
// Falls back to legacy if shader compilation fails.

class ModernRenderer {
public:
    ModernRenderer() = default;
    ~ModernRenderer();

    bool init(const char* vert_path, const char* frag_path);
    void render(const ParticleSystem& sys, GLuint texture_id,
                float zoom, int viewport_w, int viewport_h);

    bool is_ready() const { return ready_; }

private:
    bool compile_shader(GLuint shader, const char* source);
    bool load_shader_file(const char* path, char* buf, int buf_size);

    GLuint program_ = 0;
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLint loc_mvp_ = -1;
    GLint loc_texture_ = -1;
    bool ready_ = false;
};
