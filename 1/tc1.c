// 2次元図形を描く　t1.c 

#include <GLUT/glut.h>
 
void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3d(1.0, 0.0, 0.0);
  glLineWidth(2.0);          // 描画する線の太さ指定
  glBegin(GL_LINE_STRIP);    // 折れ線描画を指定
     glVertex2d(-0.8, 0.3);  // V0
     glVertex2d(-0.7, -0.1); // V1
     glVertex2d(-0.1, 0.4);  // V2
     glVertex2d( 0.1, -0.2); // V3
     glVertex2d( 0.5, 0.1);  // V4
     glVertex2d( 0.8, -0.3); // V5
  glEnd();
  glFlush();                 // 画面出力
}
 
int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glutMainLoop();
  return 0;
}
