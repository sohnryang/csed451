#pragma once

#include <tiny_obj_loader.h>

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

#include <bounding_box.hpp>

struct Model {
  GLuint vao_id;
  BoundingBox3D bounding_box;
  std::size_t index_count;

  Model() = default;
  Model(const Model &) = default;
  Model(Model &&) = default;
  Model &operator=(const Model &) = default;
  Model &operator=(Model &&) = default;
  Model(const tinyobj::attrib_t &attrib,
        const std::vector<tinyobj::shape_t> &shapes,
        const std::vector<tinyobj::material_t> &materials);
};
