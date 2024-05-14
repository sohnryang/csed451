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

#include <algorithm>
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
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 mat_ambient;
layout (location = 4) in vec3 mat_diffuse;
layout (location = 5) in vec3 mat_specular;
layout (location = 6) in float mat_shininess;
layout (location = 7) in vec2 tex_coord;

uniform vec3 light_pos;
uniform float ambient_intensity;
uniform float diffuse_intensity;
uniform float specular_intensity;
uniform mat4 projection_mat;
uniform mat4 modelview_mat;

out vec3 color_out;
out vec2 tex_coord_out;

void main() {
  vec4 pos_modelview = modelview_mat * vec4(pos, 1.0);
  gl_Position = projection_mat * pos_modelview;

  vec3 transformed_normal = normalize(modelview_mat * vec4(normal, 1.0)).xyz;
  vec3 light_direction = normalize(light_pos - pos_modelview.xyz);
  vec3 ambient = ambient_intensity * mat_ambient;
  vec3 diffuse = max(dot(light_direction, transformed_normal), 0.0) * diffuse_intensity * mat_diffuse;
  vec3 eye = normalize(-pos);
  vec3 halfway = normalize(light_direction + eye);
  float ks = pow(max(dot(transformed_normal, halfway), 0.0), mat_shininess);
  vec3 specular = specular_intensity * ks * mat_specular;
  if (dot(light_direction, transformed_normal) < 0.0) specular = vec3(0.0, 0.0, 0.0);
  color_out = ambient + diffuse + specular;
  tex_coord_out = tex_coord;
})",
             *fragment_shader = R"(#version 330 core
in vec3 color_out;
in vec2 tex_coord_out;
uniform sampler2D texture_sampler;

out vec4 FragColor;

void main() {
  FragColor = vec4(color_out, 1.0) * texture(texture_sampler, tex_coord_out);
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
  const auto modelview_mat =
      glm::translate(glm::mat4(1), {0, -0.5, 0}) *
      glm::scale(glm::mat4(1), {0.75, 0.75, 0.75}) *
      glm::rotate(glm::mat4(1), glm::radians(20 * delta), {0, 1, 0});
  const auto modelview_mat_location =
      glGetUniformLocation(program_id, "modelview_mat");
  glUniformMatrix4fv(modelview_mat_location, 1, GL_FALSE,
                     glm::value_ptr(modelview_mat));

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
  std::vector<GLfloat> buffer_data(vertex_count * 21);
  const auto buffer_elem_size = sizeof(decltype(buffer_data)::value_type);
  for (std::size_t i = 0; i < vertex_count; i++) {
    buffer_data[21 * i] = attrib.vertices[3 * i];
    buffer_data[21 * i + 1] = attrib.vertices[3 * i + 1];
    buffer_data[21 * i + 2] = attrib.vertices[3 * i + 2];
    buffer_data[21 * i + 3] = attrib.colors[3 * i];
    buffer_data[21 * i + 4] = attrib.colors[3 * i + 1];
    buffer_data[21 * i + 5] = attrib.colors[3 * i + 2];
  }
  const auto &shapes = reader.GetShapes();
  const auto &materials = reader.GetMaterials();
  std::vector<GLuint> vertex_indices;
  const auto index_elem_size = sizeof(decltype(vertex_indices)::value_type);
  for (std::size_t s = 0; s < shapes.size(); s++) {
    std::size_t index_offset = 0;
    for (std::size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      const auto fv = shapes[s].mesh.num_face_vertices[f];
      const auto material_id = shapes[s].mesh.material_ids[f];
      const auto material = materials[material_id];
      for (std::size_t v = 0; v < fv; v++) {
        const auto index = shapes[s].mesh.indices[index_offset + v];
        vertex_indices.push_back(index.vertex_index);

        if (index.normal_index >= 0) {
          buffer_data[21 * index.vertex_index + 6] =
              attrib.normals[3 * index.normal_index];
          buffer_data[21 * index.vertex_index + 7] =
              attrib.normals[3 * index.normal_index + 1];
          buffer_data[21 * index.vertex_index + 8] =
              attrib.normals[3 * index.normal_index + 2];
        }

        buffer_data[21 * index.vertex_index + 9] = material.ambient[0];
        buffer_data[21 * index.vertex_index + 10] = material.ambient[1];
        buffer_data[21 * index.vertex_index + 11] = material.ambient[2];

        buffer_data[21 * index.vertex_index + 12] = material.diffuse[0];
        buffer_data[21 * index.vertex_index + 13] = material.diffuse[1];
        buffer_data[21 * index.vertex_index + 14] = material.diffuse[2];

        buffer_data[21 * index.vertex_index + 15] = material.specular[0];
        buffer_data[21 * index.vertex_index + 16] = material.specular[1];
        buffer_data[21 * index.vertex_index + 17] = material.specular[2];

        buffer_data[21 * index.vertex_index + 18] =
            std::max(material.shininess, 1.0f);

        if (index.texcoord_index >= 0) {
          buffer_data[21 * index.vertex_index + 19] =
              attrib.texcoords[2 * index.texcoord_index];
          buffer_data[21 * index.vertex_index + 20] =
              attrib.texcoords[2 * index.texcoord_index + 1];
        }
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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 21 * buffer_elem_size,
                        nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 21 * buffer_elem_size,
                        (void *)(3 * buffer_elem_size));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 21 * buffer_elem_size,
                        (void *)(6 * buffer_elem_size));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 21 * buffer_elem_size,
                        (void *)(9 * buffer_elem_size));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 21 * buffer_elem_size,
                        (void *)(12 * buffer_elem_size));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 21 * buffer_elem_size,
                        (void *)(15 * buffer_elem_size));
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, 21 * buffer_elem_size,
                        (void *)(18 * buffer_elem_size));
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, 21 * buffer_elem_size,
                        (void *)(19 * buffer_elem_size));
  glEnableVertexAttribArray(7);
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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
               channel_count == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
               texture_data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(texture_data);

  glDepthFunc(GL_LEQUAL);
  glDepthRange(0, 1);
  glClearDepth(1);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  last_updated = std::chrono::system_clock::now();

  const auto light_pos = glm::vec3(0, 1.0, 1.0);
  const auto light_pos_location = glGetUniformLocation(program_id, "light_pos");
  glUniform3fv(light_pos_location, 1, glm::value_ptr(light_pos));

  const auto ambient_intensity_location =
      glGetUniformLocation(program_id, "ambient_intensity");
  glUniform1f(ambient_intensity_location, 0.25);

  const auto diffuse_intensity_location =
      glGetUniformLocation(program_id, "diffuse_intensity");
  glUniform1f(diffuse_intensity_location, 5);

  const auto specular_intensity_location =
      glGetUniformLocation(program_id, "specular_intensity");
  glUniform1f(specular_intensity_location, 1);

  const auto projection_mat_location =
      glGetUniformLocation(program_id, "projection_mat");
  glUniformMatrix4fv(projection_mat_location, 1, GL_FALSE,
                     glm::value_ptr(perspective_with_lookat));

  glutDisplayFunc(display);
  glutMainLoop();
}
