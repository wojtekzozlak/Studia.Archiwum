#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/glut.h>


void init(){
  glEnable(GL_DEPTH_TEST);
  glFrustum( -1.0, 1.0, -1.0, 1.0, 1.5, -20);
}

void display(){
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glBegin( GL_TRIANGLES);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(0.5, -0.5, 0);
    glVertex3f(0.5, 0.5, 0.1);
  glEnd();

  glFlush();
  return ;
}

int main(int argc, char *argv[]){
  glutInit( &argc, argv );
  glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
  glutInitWindowSize(400, 400);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Moje okno");
  init();

  glutDisplayFunc( display );


  glutMainLoop();
  return 0;
}
