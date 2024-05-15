#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>

#define __gl_h_
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <cstdint>

struct Texture {
  GLuint texture_id;

  Texture() = default;
  Texture(const Texture &) = default;
  Texture(Texture &&) = default;
  Texture &operator=(const Texture &) = default;
  Texture &operator=(Texture &&) = default;
  Texture(std::uint8_t *texture_data, int width, int height, int channel_count);
};
