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

#include <algorithm>
#include <cstddef>
#include <vector>

#include "bounding_box.hpp"

Model::Model(const tinyobj::attrib_t &attrib,
             const std::vector<tinyobj::shape_t> &shapes,
             const std::vector<tinyobj::material_t> &materials) {
  const auto vertex_count = attrib.vertices.size() / 3;
  const auto stride_units = 24;
  std::vector<GLfloat> buffer_data(stride_units * vertex_count);
  for (std::size_t i = 0; i < vertex_count; i++)
    for (std::size_t j = 0; j < 3; j++)
      buffer_data[stride_units * i + j] = attrib.vertices[3 * i + j];

  std::vector<GLuint> vertex_indices;
  for (std::size_t s = 0; s < shapes.size(); s++) {
    std::size_t index_offset = 0;
    for (std::size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      const auto fv = shapes[s].mesh.num_face_vertices[f];
      const auto material_id = shapes[s].mesh.material_ids[f];
      const auto material = materials[material_id];
      for (std::size_t v = 0; v < fv; v++) {
        const auto index = shapes[s].mesh.indices[index_offset + v];
        const auto vertex_index = index.vertex_index;
        vertex_indices.push_back(vertex_index);

        const auto normal_index = index.normal_index;
        if (normal_index >= 0)
          for (std::size_t i = 0; i < 3; i++)
            buffer_data[stride_units * vertex_index + 3 + i] =
                attrib.normals[3 * normal_index + i];

        for (std::size_t i = 0; i < 3; i++)
          buffer_data[stride_units * vertex_index + 6 + i] =
              material.ambient[i];

        for (std::size_t i = 0; i < 3; i++)
          buffer_data[stride_units * vertex_index + 9 + i] =
              material.diffuse[i];

        for (std::size_t i = 0; i < 3; i++)
          buffer_data[stride_units * vertex_index + 12 + i] =
              material.specular[i];

        buffer_data[stride_units * vertex_index + 15] =
            std::max(material.shininess, 1.0f);

        const auto texcoord_index = index.texcoord_index;
        if (texcoord_index >= 0)
          for (std::size_t i = 0; i < 2; i++)
            buffer_data[stride_units * vertex_index + 16 + i] =
                attrib.texcoords[2 * texcoord_index + i];
      }
      if (fv >= 3) {
        const auto index0 = shapes[s].mesh.indices[index_offset].vertex_index,
                   index1 =
                       shapes[s].mesh.indices[index_offset + 1].vertex_index,
                   index2 =
                       shapes[s].mesh.indices[index_offset + 2].vertex_index;
        const glm::vec3 v0 = {attrib.vertices[3 * index0],
                              attrib.vertices[3 * index0 + 1],
                              attrib.vertices[3 * index0 + 2]},
                        v1 = {attrib.vertices[3 * index1],
                              attrib.vertices[3 * index1 + 1],
                              attrib.vertices[3 * index1 + 2]},
                        v2 = {attrib.vertices[3 * index2],
                              attrib.vertices[3 * index2 + 1],
                              attrib.vertices[3 * index2 + 2]};
        const auto texindex0 =
                       shapes[s].mesh.indices[index_offset].texcoord_index,
                   texindex1 =
                       shapes[s].mesh.indices[index_offset + 1].texcoord_index,
                   texindex2 =
                       shapes[s].mesh.indices[index_offset + 2].texcoord_index;
        const glm::vec2 uv0 = {attrib.texcoords[2 * texindex0],
                               attrib.texcoords[2 * texindex0 + 1]},
                        uv1 = {attrib.texcoords[2 * texindex1],
                               attrib.texcoords[2 * texindex1 + 1]},
                        uv2 = {attrib.texcoords[2 * texindex2],
                               attrib.texcoords[2 * texindex2 + 1]};
        const auto delta_pos1 = v1 - v0, delta_pos2 = v2 - v0;
        const auto delta_uv1 = uv1 - uv0, delta_uv2 = uv2 - uv0;
        float r =
            1.0f / (delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x);
        const auto tangent =
                       (delta_pos1 * delta_uv2.y - delta_pos2 * delta_uv1.y) *
                       r,
                   bitangent =
                       (delta_pos2 * delta_uv1.x - delta_pos1 * delta_uv2.x) *
                       r;
        for (std::size_t i = 0; i < 3; i++) {
          buffer_data[stride_units * index0 + 18 + i] = tangent[i];
          buffer_data[stride_units * index1 + 18 + i] = tangent[i];
          buffer_data[stride_units * index2 + 18 + i] = tangent[i];
          buffer_data[stride_units * index0 + 21 + i] = bitangent[i];
          buffer_data[stride_units * index1 + 21 + i] = bitangent[i];
          buffer_data[stride_units * index2 + 21 + i] = bitangent[i];
        }
      }
      index_offset += fv;
    }
  }
  index_count = vertex_indices.size();
  bounding_box =
      BoundingBox3D::from_vertex_index_array(attrib.vertices, vertex_indices);

  const auto buffer_elem_size = sizeof(decltype(buffer_data)::value_type);
  GLuint buffer_id;
  glGenBuffers(1, &buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBufferData(GL_ARRAY_BUFFER, buffer_elem_size * buffer_data.size(),
               buffer_data.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  const auto index_elem_size = sizeof(decltype(vertex_indices)::value_type);
  GLuint element_buffer_id;
  glGenBuffers(1, &element_buffer_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_elem_size * vertex_indices.size(),
               vertex_indices.data(), GL_STATIC_DRAW);

  glGenVertexArrays(1, &vao_id);
  glBindVertexArray(vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_id);

  const auto stride_size = stride_units * buffer_elem_size;
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride_size, (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride_size,
                        (void *)(3 * buffer_elem_size));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride_size,
                        (void *)(6 * buffer_elem_size));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride_size,
                        (void *)(9 * buffer_elem_size));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride_size,
                        (void *)(12 * buffer_elem_size));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, stride_size,
                        (void *)(15 * buffer_elem_size));
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, stride_size,
                        (void *)(16 * buffer_elem_size));
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, stride_size,
                        (void *)(18 * buffer_elem_size));
  glEnableVertexAttribArray(7);
  glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, stride_size,
                        (void *)(21 * buffer_elem_size));
  glEnableVertexAttribArray(8);
  glBindVertexArray(0);
}
