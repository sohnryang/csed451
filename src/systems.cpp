#include "systems.hpp"

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

#include "registry.hpp"

namespace systems {
bool Render::should_apply(ecs::Context<Registry> &ctx,
                          ecs::entities::EntityId id) {
  return ctx.registry().render_infos.count(id);
}

void Render::pre_update(ecs::Context<Registry> &ctx) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Render::post_update(ecs::Context<Registry> &ctx) { glutSwapBuffers(); }

void Render::update_single(ecs::Context<Registry> &ctx,
                           ecs::entities::EntityId id) {
  const auto &registry = ctx.registry();

  const auto &render_info = registry.render_infos.at(id);
  const auto &color = render_info.color;
  glColor3f(color.r, color.g, color.b);

  glPushMatrix();
  if (registry.transforms.count(id)) {
    const auto &transform = registry.transforms.at(id);
    glm::mat4 mat = glm::translate(glm::mat4(1), transform.disp);
    glLoadMatrixf(glm::value_ptr(mat));
  } else
    glLoadIdentity();

  glBegin(GL_POLYGON);
  const auto &vertices = render_info.vertices;
  for (const auto &v : vertices)
    glVertex3f(v[0], v[1], v[2]);
  glEnd();
  glPopMatrix();
}

Render::Render() {
  glClearColor(0, 0, 0, 1);
  glDepthFunc(GL_LESS);
  glDepthRange(0, 1);
  glClearDepth(1);
}
} // namespace systems
