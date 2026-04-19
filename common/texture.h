#pragma once

#ifdef __APPLE__
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

// Load an image file (BMP, PNG, JPG) as an OpenGL texture.
// Returns the GL texture ID, or 0 on failure.
GLuint load_texture(const char* path);
