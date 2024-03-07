#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <gl/glut.h>
#endif

struct rect {
  float x;
  float y;
  float width;
  float height;
};
rect rectangle;

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  rectangle.x = 0.45;
  rectangle.y = 0.48;
  rectangle.width = 0.1;
  rectangle.height = 0.15;
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0);

  glBegin(GL_LINE_LOOP);
  glVertex2f(rectangle.x, rectangle.y);
  glVertex2f(rectangle.x, rectangle.y + rectangle.height);
  glVertex2f(rectangle.x + rectangle.width, rectangle.y + rectangle.height);
  glVertex2f(rectangle.x + rectangle.width, rectangle.y);
  glEnd();

  glutSwapBuffers();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, 1, 0, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 'i':
    rectangle.y += 0.005;
    break;
  case 'm':
    rectangle.y -= 0.005;
    break;
  case 'k':
    rectangle.x += 0.005;
    break;
  case 'j':
    rectangle.x -= 0.005;
    break;
  }
  glutPostRedisplay();
}

void special_keyboard(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_UP:
    rectangle.y += 0.005;
    break;
  case GLUT_KEY_DOWN:
    rectangle.y -= 0.005;
    break;
  case GLUT_KEY_RIGHT:
    rectangle.x += 0.005;
    break;
  case GLUT_KEY_LEFT:
    rectangle.x -= 0.005;
    break;
  }
  glutPostRedisplay();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  init();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special_keyboard);
  glutMainLoop();
}
