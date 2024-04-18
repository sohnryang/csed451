#pragma once

#include <ecs/entities.hpp>
#include <ecs/systems.hpp>

#include <cmath>

#include <components.hpp>
#include <grid.hpp>
#include <registry.hpp>

const float PI = std::acos(-1);

void load_models(ecs::Context<Registry> &ctx);

void setup_camera(ecs::Context<Registry> &ctx);

void create_character(ecs::Context<Registry> &ctx);
