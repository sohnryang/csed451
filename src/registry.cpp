#include "registry.hpp"

#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <cstddef>
#include <iostream>
#include <utility>

#include "components.hpp"

Registry::Registry() : models(model_filenames.size()) {
  for (std::size_t i = 0; i < model_filenames.size(); i++) {
    model_indices[model_filenames[i]] = i;

    const auto &filename = model_filenames[i];
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
    std::vector<glm::vec3> vertices;
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
          vertices.push_back(glm::vec3(vx, vy, vz));
        }
        index_offset += fv;
      }
    }
    models[i] = Model(std::move(vertices));

    std::cout << "Loaded obj file: " << filename << std::endl;
  }
}

ecs::entities::EntityId Registry::add_mesh(ecs::Context<Registry> &ctx,
                                           components::Mesh &&mesh) {
  auto id = ctx.entity_manager().next_id();
  meshes[id] = std::move(mesh);
  return id;
}

TileType Registry::random_tile_type(ecs::Context<Registry> &ctx) {
  return static_cast<TileType>(random_tile_type_dist(ctx.random_gen()));
}

int Registry::random_tile_length(ecs::Context<Registry> &ctx) {
  return random_tile_length_dist(ctx.random_gen());
}

int Registry::random_column(ecs::Context<Registry> &ctx) {
  return random_column_dist(ctx.random_gen());
}

int Registry::random_column(ecs::Context<Registry> &ctx,
                            std::vector<bool> &ref) {
  while (true) {
    int ret = random_column_dist(ctx.random_gen());
    if (!ref[ret])
      return ret;
  }
}

int Registry::random_tree_number(ecs::Context<Registry> &ctx) {
  return random_tree_number_dist(ctx.random_gen());
}

double Registry::random_speed(ecs::Context<Registry> &ctx) {
  while (true) {
    double ret = random_speed_dist(ctx.random_gen());
    if (std::abs(ret) < 0.5)
      continue;
    return ret;
  }
}

bool Registry::random_probability(ecs::Context<Registry> &ctx, double p) {
  return random_probability_dist(ctx.random_gen()) <= p;
}
