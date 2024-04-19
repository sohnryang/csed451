#include <scene.hpp>

#include <ecs/systems.hpp>

#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <cstddef>
#include <iostream>
#include <stdexcept>

#include <components.hpp>
#include <grid.hpp>
#include <registry.hpp>
#include <systems.hpp>

void load_models(ecs::Context<Registry> &ctx) {
  for (const auto &filename : ctx.registry().model_filenames) {
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./";
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filename, reader_config))
      throw std::runtime_error("obj file parse failed");

    if (!reader.Warning().empty())
      std::cout << "TinyObjReader: " << reader.Warning();

    const auto &attrib = reader.GetAttrib();
    const auto &shapes = reader.GetShapes();
    const auto &materials = reader.GetMaterials();
    for (std::size_t s = 0; s < shapes.size(); s++) {
      std::size_t index_offset = 0;
      for (std::size_t f = 0; f < shapes[s].mesh.num_face_vertices.size();
           f++) {
        const auto fv = shapes[s].mesh.num_face_vertices[f];
        for (std::size_t v = 0; v < fv; v++) {
          const auto idx = shapes[s].mesh.indices[index_offset + v];
          const auto vx = attrib.vertices[3 * idx.vertex_index],
                     vy = attrib.vertices[3 * idx.vertex_index + 1],
                     vz = attrib.vertices[3 * idx.vertex_index + 2];
          ctx.registry().model_vertices[filename].push_back(
              glm::vec3(vx, vy, vz));
        }
        index_offset += fv;
      }
    }

    std::cout << "Loaded obj file: " << filename << std::endl;
  }
}

void setup_camera(ecs::Context<Registry> &ctx) {
  ctx.registry().camera_config = {glm::vec3(0, 0, 3),
                                  glm::vec3(0, 0, 0),
                                  glm::vec3(0, 1, 0),
                                  40,
                                  1,
                                  0.1,
                                  10};
}

void create_character(ecs::Context<Registry> &ctx) {
  const auto character_vertices = ctx.registry().model_vertices["rooster.obj"];
  const auto id =
      ctx.registry().add_mesh(ctx, {character_vertices, glm::mat4(1)});
  ctx.registry().character_id = id;
  ctx.registry().animations[id] = {components::AnimationState::BEFORE_START,
                                   {components::AnimationKind::DISABLED, {}},
                                   glm::mat4(1),
                                   0};
}
