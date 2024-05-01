#include "shader_program.hpp"

#ifdef __APPLE__
#include <OpenGL/gl3.h>

#define __gl_h_
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

void load_shader(GLuint shader_id, const std::string &filename) {
  std::ifstream infile(filename);
  std::stringstream stream;
  stream << infile.rdbuf();
  std::string content = stream.str();
  const char *raw_content = content.c_str();
  glShaderSource(shader_id, 1, &raw_content, nullptr);
  glCompileShader(shader_id);
  GLint compile_status;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
  if (compile_status == GL_FALSE)
    throw std::runtime_error("shader compilation failed");
}

ShaderProgram::ShaderProgram(const std::string &vertex_shader_filename,
                             const std::string &fragment_shader_filename) {
  program_id = glCreateProgram();

  GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  load_shader(vertex_shader_id, vertex_shader_filename);
  glAttachShader(program_id, vertex_shader_id);

  GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
  load_shader(fragment_shader_id, fragment_shader_filename);
  glAttachShader(program_id, fragment_shader_id);

  glLinkProgram(program_id);
  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);
}
