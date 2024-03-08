#include "ecs/entities.hpp"
#include "ecs/systems.hpp"

#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <gl/glut.h>
#endif

#include <cmath>
#include <iterator>
#include <memory>
#include <random>
#include <unordered_map>
#include <vector>

const double PI = std::acos(-1);

namespace components {
struct Polygon {
  std::vector<glm::vec4> vertices;
};

struct Color {
  float r;
  float g;
  float b;
};

struct Transform {
  glm::vec3 displacement;
  glm::vec3 velocity;
};
}; // namespace components

struct Registry {
  std::unordered_map<ecs::entities::EntityId, components::Polygon> polygons;
  std::unordered_map<ecs::entities::EntityId, components::Color> colors;
  std::unordered_map<ecs::entities::EntityId, components::Transform> transforms;

  void add_moving_polygon(ecs::Context<Registry> &ctx,
                          const components::Polygon &polygon,
                          const components::Color &color,
                          const components::Transform transform) {
    const auto id = ctx.entity_manager().next_id();
    polygons[id] = polygon;
    colors[id] = color;
    transforms[id] = transform;
  }
};

class Transform : public ecs::systems::System<Registry> {
public:
  bool should_apply(ecs::Context<Registry> &ctx,
                    ecs::entities::EntityId id) override {
    return ctx.registry().transforms.count(id);
  }

  void update_single(ecs::Context<Registry> &ctx,
                     ecs::entities::EntityId id) override {

    auto &disp = ctx.registry().transforms[id].displacement;
    auto &vel = ctx.registry().transforms[id].velocity;
    disp += ctx.delta_time() * ctx.registry().transforms[id].velocity;
    if (disp[0] < -1)
      vel[0] = abs(vel[0]);
    else if (disp[0] > 1)
      vel[0] = -abs(vel[0]);
    if (disp[1] < -1)
      vel[1] = abs(vel[1]);
    else if (disp[1] > 1)
      vel[1] = -abs(vel[1]);
  }
};

class Render : public ecs::systems::System<Registry> {
public:
  Render() {
    glClearColor(0, 0, 0, 1);
    glDepthFunc(GL_LESS);
    glDepthRange(0, 1);
    glClearDepth(1);
  }

  bool should_apply(ecs::Context<Registry> &ctx,
                    ecs::entities::EntityId id) override {
    return ctx.registry().polygons.count(id) && ctx.registry().colors.count(id);
  }

  void pre_update(ecs::Context<Registry> &ctx) override {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void post_update(ecs::Context<Registry> &ctx) override { glutSwapBuffers(); }

  void update_single(ecs::Context<Registry> &ctx,
                     ecs::entities::EntityId id) override {
    auto &registry = ctx.registry();
    glColor3f(registry.colors[id].r, registry.colors[id].g,
              registry.colors[id].b);
    glPushMatrix();
    if (registry.transforms.count(id)) {
      glm::mat4 mat =
          glm::translate(glm::mat4(1), registry.transforms[id].displacement);
      glLoadMatrixf(glm::value_ptr(mat));
    } else
      glLoadIdentity();
    glBegin(GL_POLYGON);
    for (const auto &v : registry.polygons[id].vertices)
      glVertex3f(v[0], v[1], v[2]);
    glEnd();
    glPopMatrix();
  }
};

std::shared_ptr<ecs::Context<Registry>> ctx_ptr;

void display() { ctx_ptr->update(); }

void idle() { glutPostRedisplay(); }

template <typename G>
std::vector<glm::vec4> random_n_gon(G &gen, int n, float radius) {
  std::uniform_real_distribution<> dist(0, 2 * PI);
  std::vector<float> angles;
  for (int i = 0; i < n; i++)
    angles.push_back(dist(gen));
  std::sort(angles.begin(), angles.end());
  std::vector<glm::vec4> vertices;
  std::transform(angles.begin(), angles.end(), std::back_inserter(vertices),
                 [radius](auto angle) {
                   return glm::vec4(radius * cos(angle), radius * sin(angle), 0,
                                    1);
                 });
  return vertices;
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(512, 512);
  glutCreateWindow("ECS Demo");

  std::vector<std::shared_ptr<ecs::systems::System<Registry>>> systems;
  systems.emplace_back(new Transform);
  systems.emplace_back(new Render);
  ctx_ptr =
      std::make_shared<ecs::Context<Registry>>(Registry(), std::move(systems));

  const int polygon_count = 100;
  std::random_device rd;
  std::default_random_engine gen(rd());
  for (int i = 0; i < polygon_count; i++) {
    std::uniform_int_distribution<> vertex_dist(3, 8);
    auto vertices = random_n_gon(gen, vertex_dist(gen), 0.1);
    components::Polygon polygon = {vertices};

    std::uniform_real_distribution<float> color_dist(0, 1);
    components::Color color = {color_dist(gen), color_dist(gen),
                               color_dist(gen)};

    std::uniform_real_distribution<float> coord_dist(-1, 1);
    components::Transform transform = {
        glm::vec3(coord_dist(gen), coord_dist(gen), 0),
        glm::vec3(coord_dist(gen), coord_dist(gen), 0)};

    ctx_ptr->registry().add_moving_polygon(*ctx_ptr, polygon, color, transform);
  }

  glutDisplayFunc(display);
  glutIdleFunc(idle);
  glutMainLoop();
}
