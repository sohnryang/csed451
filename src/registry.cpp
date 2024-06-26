#include "registry.hpp"

#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#ifdef __APPLE__
#include <OpenGL/gl3.h>

#define __gl_h_
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <utility>

#include "components.hpp"
#include "shader_program.hpp"
#include "texture.hpp"

Registry::Registry()
    : models(model_filenames.size()), 
      textures(texture_filenames.size() + normal_filenames.size()),
      shader_programs(2) {
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

    models[i] =
        Model(reader.GetAttrib(), reader.GetShapes(), reader.GetMaterials());

    shader_programs[GOURAUD_SHADER] =
        ShaderProgram("gouraud.vert", "gouraud.frag");
    shader_programs[PHONG_SHADER] = ShaderProgram("phong.vert", "phong.frag");

    std::cout << "Loaded obj file: " << filename << std::endl;
  }

  stbi_set_flip_vertically_on_load(true);
  for (std::size_t i = 0; i < texture_filenames.size(); i++) {
    const auto filename = texture_filenames[i];
    texture_indicies[filename] = i;

    int width, height, channel_count;
    std::uint8_t *texture_data =
        stbi_load(filename.c_str(), &width, &height, &channel_count, 0);
    if (texture_data == nullptr)
      throw std::runtime_error("texture load failed");
    textures[i] = Texture(texture_data, width, height, channel_count);
    stbi_image_free(texture_data);

    std::cout << "Loaded texture file: " << filename << std::endl;
  }

  stbi_set_flip_vertically_on_load(true);
  for (std::size_t i = 0; i < normal_filenames.size(); i++) {
    auto idx = texture_filenames.size() + i;
    const auto filename = normal_filenames[i];
    texture_indicies[filename] = idx;

    int width, height, channel_count;
    std::uint8_t *texture_data =
        stbi_load(filename.c_str(), &width, &height, &channel_count, 0);
    if (texture_data == nullptr)
      throw std::runtime_error("texture load failed");
    textures[idx] = Texture(texture_data, width, height, channel_count);
    stbi_image_free(texture_data);

    std::cout << "Loaded texture file: " << filename << std::endl;
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
