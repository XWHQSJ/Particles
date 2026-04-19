#include "renderer_modern.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#elif defined(__APPLE__)
#include <OpenGL/gl3.h>
#elif defined(USE_GLEW)
#include <GL/glew.h>
#else
#include <GL/gl.h>
#endif

// Embedded shaders as fallback
// Use GLSL ES 300 for Emscripten, GLSL 330 core for desktop
#ifdef __EMSCRIPTEN__
static const char* kDefaultVertexShader = R"(#version 300 es
precision highp float;
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in float aSize;

uniform mat4 uMVP;

out vec4 vColor;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    gl_PointSize = aSize;
    vColor = aColor;
}
)";

static const char* kDefaultFragmentShader = R"(#version 300 es
precision highp float;
in vec4 vColor;
out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    vec4 texColor = texture(uTexture, gl_PointCoord);
    FragColor = texColor * vColor;
}
)";
#else
static const char* kDefaultVertexShader = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in float aSize;

uniform mat4 uMVP;

out vec4 vColor;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    gl_PointSize = aSize;
    vColor = aColor;
}
)";

static const char* kDefaultFragmentShader = R"(
#version 330 core
in vec4 vColor;
out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    vec4 texColor = texture(uTexture, gl_PointCoord);
    FragColor = texColor * vColor;
}
)";
#endif

struct ParticleVertex {
    float x, y, z;
    float r, g, b, a;
    float size;
};

ModernRenderer::~ModernRenderer() {
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
    if (program_) glDeleteProgram(program_);
}

bool ModernRenderer::compile_shader(GLuint shader, const char* source) {
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        fprintf(stderr, "Shader compile error: %s\n", log);
        return false;
    }
    return true;
}

bool ModernRenderer::load_shader_file(const char* path, char* buf,
                                      int buf_size) {
    FILE* f = fopen(path, "r");
    if (!f) return false;
    size_t n = fread(buf, 1, static_cast<size_t>(buf_size - 1), f);
    buf[n] = '\0';
    fclose(f);
    return n > 0;
}

bool ModernRenderer::init(const char* vert_path, const char* frag_path) {
#ifdef USE_GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "GLEW init failed: %s\n",
                reinterpret_cast<const char*>(glewGetErrorString(err)));
        return false;
    }
#endif

    char vert_src[4096];
    char frag_src[4096];

    const char* vs = kDefaultVertexShader;
    const char* fs = kDefaultFragmentShader;

    if (vert_path && load_shader_file(vert_path, vert_src, 4096)) {
        vs = vert_src;
    }
    if (frag_path && load_shader_file(frag_path, frag_src, 4096)) {
        fs = frag_src;
    }

    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

    if (!compile_shader(vert, vs) || !compile_shader(frag, fs)) {
        glDeleteShader(vert);
        glDeleteShader(frag);
        return false;
    }

    program_ = glCreateProgram();
    glAttachShader(program_, vert);
    glAttachShader(program_, frag);
    glLinkProgram(program_);

    GLint success = 0;
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program_, 512, nullptr, log);
        fprintf(stderr, "Shader link error: %s\n", log);
        glDeleteShader(vert);
        glDeleteShader(frag);
        glDeleteProgram(program_);
        program_ = 0;
        return false;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);

    loc_mvp_ = glGetUniformLocation(program_, "uMVP");
    loc_texture_ = glGetUniformLocation(program_, "uTexture");

    // Create VAO + VBO
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(ParticleVertex),
                          reinterpret_cast<void*>(0));
    // color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
                          sizeof(ParticleVertex),
                          reinterpret_cast<void*>(3 * sizeof(float)));
    // size
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,
                          sizeof(ParticleVertex),
                          reinterpret_cast<void*>(7 * sizeof(float)));

    glBindVertexArray(0);

    ready_ = true;
    return true;
}

// Simple perspective matrix
static void perspective_matrix(float* m, float fov_deg, float aspect,
                               float near, float far) {
    memset(m, 0, 16 * sizeof(float));
    float f = 1.0f / tanf(fov_deg * 3.14159265f / 360.0f);
    m[0] = f / aspect;
    m[5] = f;
    m[10] = (far + near) / (near - far);
    m[11] = -1.0f;
    m[14] = (2.0f * far * near) / (near - far);
}

void ModernRenderer::render(const ParticleSystem& sys, GLuint texture_id,
                            float zoom, int vp_w, int vp_h) {
    if (!ready_) return;

    const auto& parts = sys.particles();
    std::vector<ParticleVertex> verts;
    verts.reserve(parts.size());

    for (const auto& p : parts) {
        if (!p.active) continue;
        ParticleVertex v;
        v.x = p.pos.x;
        v.y = p.pos.y;
        v.z = p.pos.z + zoom;
        v.r = p.color.r;
        v.g = p.color.g;
        v.b = p.color.b;
        v.a = p.life;
        v.size = 8.0f * p.life;
        verts.push_back(v);
    }

    if (verts.empty()) return;

    glUseProgram(program_);

    // MVP: just perspective, no model/view transform needed since
    // we already baked zoom into z
    float mvp[16];
    float aspect = (vp_h > 0) ? static_cast<float>(vp_w) / vp_h : 1.0f;
    perspective_matrix(mvp, 45.0f, aspect, 0.1f, 200.0f);
    glUniformMatrix4fv(loc_mvp_, 1, GL_FALSE, mvp);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glUniform1i(loc_texture_, 0);

    // Upload vertex data
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(verts.size() * sizeof(ParticleVertex)),
                 verts.data(), GL_DYNAMIC_DRAW);

#ifndef __EMSCRIPTEN__
    glEnable(GL_PROGRAM_POINT_SIZE);
#endif
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(verts.size()));

    glBindVertexArray(0);
    glUseProgram(0);
}
