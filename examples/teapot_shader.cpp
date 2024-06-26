#ifdef __APPLE__
#include <OpenGL/gl3.h>

#define __gl_h_
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

GLuint program_id, vertex_shader_id, fragment_shader_id, vao_id;
std::size_t index_count;
std::chrono::time_point<std::chrono::system_clock> last_updated;
const GLchar *vertex_shader = R"(#version 330 core
layout (location = 0) in vec3 pos;
uniform mat4 transform_mat;

void main() {
  gl_Position = transform_mat * vec4(pos.x, pos.y, pos.z, 1.0);
})",
             *fragment_shader = R"(#version 330 core
out vec4 FragColor;

void main() {
  FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
})";
glm::mat4 perspective_with_lookat =
    glm::perspective(glm::radians(40.0f), 1.0f, 0.1f, 5.0f) *
    glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

void display() {
  const auto now = std::chrono::system_clock::now();
  const auto duration = now - last_updated;
  const auto delta =
      std::chrono::duration_cast<std::chrono::duration<float>>(duration)
          .count();
  const auto transform_mat =
      perspective_with_lookat * glm::translate(glm::mat4(1), {0, -0.5, 0}) *
      glm::scale(glm::mat4(1), {0.3, 0.3, 0.3}) *
      glm::rotate(glm::mat4(1), glm::radians(20 * delta), {0, 1, 0});
  const auto transform_mat_location =
      glGetUniformLocation(program_id, "transform_mat");
  glUniformMatrix4fv(transform_mat_location, 1, GL_FALSE,
                     glm::value_ptr(transform_mat));

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glBindVertexArray(vao_id);
  glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  glutSwapBuffers();
  glutPostRedisplay();
}

int main(int argc, char **argv) {
  std::string filename = "teapot.obj";
  tinyobj::ObjReaderConfig reader_config;
  reader_config.mtl_search_path = "./";
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(filename, reader_config)) {
    if (!reader.Error().empty()) {
      std::cerr << "TinyObjReader: " << reader.Error();
    }
    std::exit(1);
  }

  if (!reader.Warning().empty()) {
    std::cout << "TinyObjReader: " << reader.Warning();
  }

  glutInit(&argc, argv);
#ifdef __APPLE__
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH |
                      GLUT_3_2_CORE_PROFILE);
#else
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
#endif
  glutInitWindowSize(512, 512);
  glutCreateWindow("Teapot");

#ifndef __APPLE__
  GLenum init_status = glewInit();
  if (init_status != GLEW_OK) {
    std::cerr << "GLEW init error: " << glewGetErrorString(init_status)
              << std::endl;
    std::exit(1);
  }
#endif
  program_id = glCreateProgram();

  vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader_id, 1, &vertex_shader, nullptr);
  glCompileShader(vertex_shader_id);
  GLint vertex_shader_compile_status;
  glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS,
                &vertex_shader_compile_status);
  if (vertex_shader_compile_status == GL_FALSE) {
    std::cerr << "Vertex shader compilation failed" << std::endl;
    std::exit(1);
  }
  glAttachShader(program_id, vertex_shader_id);

  fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader_id, 1, &fragment_shader, nullptr);
  glCompileShader(fragment_shader_id);
  GLint fragment_shader_compile_status;
  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS,
                &fragment_shader_compile_status);
  if (fragment_shader_compile_status == GL_FALSE) {
    std::cerr << "Fragment shader compilation failed" << std::endl;
    std::exit(1);
  }
  glAttachShader(program_id, fragment_shader_id);

  glLinkProgram(program_id);
  glUseProgram(program_id);
  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);

  std::cout << "Loaded obj file" << std::endl;

  const auto &attrib = reader.GetAttrib();
  GLuint buffer_id;
  glGenBuffers(1, &buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * attrib.vertices.size(),
               &attrib.vertices[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  const auto &shapes = reader.GetShapes();
  GLuint element_buffer_id;
  std::vector<GLuint> vertex_indices;
  for (std::size_t s = 0; s < shapes.size(); s++) {
    std::size_t index_offset = 0;
    for (std::size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      const auto fv = shapes[s].mesh.num_face_vertices[f];
      for (std::size_t v = 0; v < fv; v++)
        vertex_indices.push_back(
            shapes[s].mesh.indices[index_offset + v].vertex_index);
      index_offset += fv;
    }
  }
  index_count = vertex_indices.size();
  glGenBuffers(1, &element_buffer_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertex_indices.size(),
               &vertex_indices[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  std::cout << "Loaded to buffer" << std::endl;

  glGenVertexArrays(1, &vao_id);
  glBindVertexArray(vao_id);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_id);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindVertexArray(0);

  last_updated = std::chrono::system_clock::now();

  glutDisplayFunc(display);
  glutMainLoop();
}
