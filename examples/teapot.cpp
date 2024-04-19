#include <chrono>
#include <cstddef>
#include <iostream>
#include <string>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <gl/glut.h>
#endif

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

tinyobj::ObjReader reader;
std::chrono::time_point<std::chrono::system_clock> last_updated;

void display() {
  const auto now = std::chrono::system_clock::now();
  const auto duration = now - last_updated;
  const auto delta =
      std::chrono::duration_cast<std::chrono::duration<float>>(duration)
          .count();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(1, 1, 1);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glPushMatrix();
  glTranslatef(0, -0.5, 0);
  glScalef(0.3, 0.3, 0.3);
  glRotatef(20 * delta, 0, 1, 0);

  auto &attrib = reader.GetAttrib();
  auto &shapes = reader.GetShapes();
  auto &materials = reader.GetMaterials();
  glBegin(GL_TRIANGLES);
  for (std::size_t s = 0; s < shapes.size(); s++) {
    std::size_t index_offset = 0;
    for (std::size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      const auto fv = shapes[s].mesh.num_face_vertices[f];

      for (std::size_t v = 0; v < fv; v++) {
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index],
                        vy = attrib.vertices[3 * idx.vertex_index + 1],
                        vz = attrib.vertices[3 * idx.vertex_index + 2];
        glVertex3f(vx, vy, vz);
      }

      index_offset += fv;
    }
  }
  glEnd();
  glPopMatrix();

  glutSwapBuffers();
  glutPostRedisplay();
}

int main(int argc, char **argv) {
  std::string filename = "teapot.obj";
  tinyobj::ObjReaderConfig reader_config;
  reader_config.mtl_search_path = "./";

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
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(512, 512);
  glutCreateWindow("Teapot");

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40, 1, 0.1, 5);

  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);

  last_updated = std::chrono::system_clock::now();

  glutDisplayFunc(display);
  glutMainLoop();
}
