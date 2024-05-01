#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>

#define __gl_h_
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <string>

struct ShaderProgram {
  GLuint program_id;

  ShaderProgram() = default;
  ShaderProgram(const ShaderProgram &) = default;
  ShaderProgram(ShaderProgram &&) = default;
  ShaderProgram &operator=(const ShaderProgram &) = default;
  ShaderProgram &operator=(ShaderProgram &&) = default;
  ShaderProgram(const std::string &vertex_shader_filename,
                const std::string &fragment_shader_filename);
};
