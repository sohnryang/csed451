#include <cstdint>
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

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <chrono>
#include <cstddef>
#include <iostream>
#include <string>

GLuint program_id, vertex_shader_id, fragment_shader_id, vao_id, texture_id;
std::size_t index_count;
std::chrono::time_point<std::chrono::system_clock> last_updated;
const GLchar *vertex_shader = R"(#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_coord;
uniform mat4 transform_mat;

out vec3 color_out;
out vec2 tex_coord_out;

void main() {
  gl_Position = transform_mat * vec4(pos.x, pos.y, pos.z, 1.0);
  color_out = color;
  tex_coord_out = tex_coord;
})",
             *fragment_shader = R"(#version 330 core
in vec3 color_out;
in vec2 tex_coord_out;
uniform sampler2D texture_sampler;

out vec4 FragColor;

void main() {
  FragColor = texture(texture_sampler, tex_coord_out);
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
      glm::scale(glm::mat4(1), {0.75, 0.75, 0.75}) *
      glm::rotate(glm::mat4(1), glm::radians(20 * delta), {0, 1, 0});
  const auto transform_mat_location =
      glGetUniformLocation(program_id, "transform_mat");
  glUniformMatrix4fv(transform_mat_location, 1, GL_FALSE,
                     glm::value_ptr(transform_mat));

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glBindVertexArray(vao_id);
  glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  glutSwapBuffers();
  glutPostRedisplay();
}

int main(int argc, char **argv) {
  const std::string filename = "car.obj";
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
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE |
                      GLUT_3_2_CORE_PROFILE);
#else
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
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
  const auto vertex_count = attrib.vertices.size() / 3;
  std::vector<GLfloat> buffer_data(vertex_count * 8);
  const auto buffer_elem_size = sizeof(decltype(buffer_data)::value_type);
  for (std::size_t i = 0; i < vertex_count; i++) {
    buffer_data[8 * i] = attrib.vertices[3 * i];
    buffer_data[8 * i + 1] = attrib.vertices[3 * i + 1];
    buffer_data[8 * i + 2] = attrib.vertices[3 * i + 2];
    buffer_data[8 * i + 3] = attrib.colors[3 * i];
    buffer_data[8 * i + 4] = attrib.colors[3 * i + 1];
    buffer_data[8 * i + 5] = attrib.colors[3 * i + 2];
  }
  const auto &shapes = reader.GetShapes();
  std::vector<GLuint> vertex_indices;
  const auto index_elem_size = sizeof(decltype(vertex_indices)::value_type);
  for (std::size_t s = 0; s < shapes.size(); s++) {
    std::size_t index_offset = 0;
    for (std::size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      const auto fv = shapes[s].mesh.num_face_vertices[f];
      for (std::size_t v = 0; v < fv; v++) {
        const auto index = shapes[s].mesh.indices[index_offset + v];
        vertex_indices.push_back(index.vertex_index);
        if (index.texcoord_index < 0)
          continue;
        buffer_data[8 * index.vertex_index + 6] =
            attrib.texcoords[2 * index.texcoord_index];
        buffer_data[8 * index.vertex_index + 7] =
            attrib.texcoords[2 * index.texcoord_index + 1];
      }
      index_offset += fv;
    }
  }

  GLuint buffer_id;
  glGenBuffers(1, &buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBufferData(GL_ARRAY_BUFFER, buffer_elem_size * buffer_data.size(),
               buffer_data.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  GLuint element_buffer_id;
  index_count = vertex_indices.size();
  glGenBuffers(1, &element_buffer_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_elem_size * vertex_indices.size(),
               vertex_indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  std::cout << "Loaded to buffer" << std::endl;

  glGenVertexArrays(1, &vao_id);
  glBindVertexArray(vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_id);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * buffer_elem_size,
                        nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * buffer_elem_size,
                        (void *)(3 * buffer_elem_size));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * buffer_elem_size,
                        (void *)(6 * buffer_elem_size));
  glEnableVertexAttribArray(2);
  glBindVertexArray(0);

  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  int width, height, channel_count;
  stbi_set_flip_vertically_on_load(true);
  std::uint8_t *texture_data =
      stbi_load("car_texture.png", &width, &height, &channel_count, 0);
  if (texture_data == nullptr) {
    std::cerr << "Texture file read failed" << std::endl;
    std::exit(1);
  }
  std::cout << "Texture loaded with " << channel_count << " channels"
            << std::endl;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, texture_data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(texture_data);

  glDepthFunc(GL_LEQUAL);
  glDepthRange(0, 1);
  glClearDepth(1);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  last_updated = std::chrono::system_clock::now();

  glutDisplayFunc(display);
  glutMainLoop();
}
