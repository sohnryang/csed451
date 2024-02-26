#define GL_SILENCE_DEPRECATION

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void reshape(int w, int h) {
  glViewport(0, 0, w, h);
  glLoadIdentity();
  gluOrtho2D(0.0, 100.0, 0.0, 100.0);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(0.0, 1.0, 1.0);
  glRectf(10.0, 10.0, 90.0, 90.0);
  glutSwapBuffers();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutCreateWindow("simple");
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutMainLoop();
}
