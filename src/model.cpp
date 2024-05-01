#include "model.hpp"

#include <glm/glm.hpp>

#ifdef __APPLE__
#include <OpenGL/gl3.h>

#define __gl_h_
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <cstddef>
#include <vector>

#include "bounding_box.hpp"

Model::Model(const std::vector<float> &vertices,
             const std::vector<GLuint> &indices)
    : index_count(indices.size()) {
  bounding_box = BoundingBox3D::from_vertex_index_array(vertices, indices);

  GLuint buffer_id;
  glGenBuffers(1, &buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  GLuint element_buffer_id;
  glGenBuffers(1, &element_buffer_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(),
               indices.data(), GL_STATIC_DRAW);

  glGenVertexArrays(1, &vao_id);
  glBindVertexArray(vao_id);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_id);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindVertexArray(0);
}
