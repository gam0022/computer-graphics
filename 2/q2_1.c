/*  課題 2-1　右斜め上45°までの傾きの線分を描く関数line( )と、1/8円を描く関数octant( )
    を、それぞれBresenham, Michenerのアルゴリズムの原型部分で作成して挿入して実行するこ
    と。

    （注）Bresenhamのアルゴリズムの原型部分（0°から45°の方向まで）、Michenerのアルゴリ
    ズムの原型部分（上方向(90°)位置から右側へ45°までの範囲のみ。
    */

#include <GL/glut.h> 
#include <math.h> 
int  width=600, height=600; // 初期ウィンドウの大きさ  

void  ploti(int x, int y) 
{ 
  glBegin(GL_POINTS); glVertex2i(x,y); glEnd(); 
} 

void  line(int x1, int y1, int x2, int y2) 
{ 
  // ここに、1点をプロットする関数 ploti(x,y) を使って 
  //　(x1,y1)から(x2,y2)まで直線をプロットする 
  //　Bresenham のアルゴリズムの原型部分を入れる 
  //  整数の加減算、乗算のみ、組み込み関数は使わないこと 

  // init
  int dx = x2 - x1;
  int dy = y2 - y1;
  int d = 2*dy;
  int e = -dx;

  int x, y = y1;
  ploti(x,y);

  // main
  for(x = x1; x <= x2; x++) {
    e += d;
    if (e > 0) {
      y++;
      e -= 2*dx;
    }
    ploti(x,y);
  }
} 

void  octant(int r) 
{ 
  // ここに、1点をプロットする関数 ploti(x,y) を使って 
  //　原点を中心とし, 半径r の1/8円をプロットする 
  //　Michener のアルゴリズムの原型部分を入れる  
  //  整数の加減算、乗算のみ、組み込み関数は使わないこと 

  // init
  int x = 0;
  int y = r;
  int d = 3 - 2*r;

  ploti(x,y);

  // main
  while(x<y) {
    if (d<0) {
      d += 4*x + 6;
    } else {
      d += 4*(x-y) + 10;
      y--;
    }
    x++;
    ploti(x,y);
  }
} 

void  display(void) 
{ 
  int i,x,y,x2; 
  double a=1.99239, u=0., v=0.087156, w; 
  double p=1., q=0.99619, r, m=400.; 

  glClear(GL_COLOR_BUFFER_BIT); 
  line( 0,0,(int)m,0 ); 
  line( 0,0,(int)(m*q),(int)(m*v) ); 
  for( i=5; i<45; i+=5 ){ 
    w = a*v - u;  r = a*q - p; 
    x = (int)(m*r); y = (int)(m*w);  line( 0,0,x,y ); 
    u = v; v = w; p = q; q = r; 
  } 
  for( i=20; i<=400; i+=20 ) octant( i ); 
  glFlush(); 
} 

void myinit(void)  
{ 
  glClearColor (1.0, 1.0, 1.0, 0.0); /* background color */ 
  glColor3f(0.,0.,0.);             /* drawing color */ 
} 

void  reshape(int w, int h) 
{ 
  h = (h == 0) ? 1 : h; 
  glViewport(0, 0, w, h); 
  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity(); 
  width = w; height = h; 
  glOrtho(-100.,width-101.,-100.,height-101.,-1.,1. ); 
  glMatrixMode(GL_MODELVIEW); 
} 

void keyboard(unsigned char key, int x, int y) 
{ 
  switch (key) { 
    case 27:  /* ESC code */ 
    case 13:  exit(0);  /* RETURN code */ 
    default:  break; 
  } 
} 

int  main(int argc, char** argv) 
{ 
  glutInitWindowSize (width, height);  
  glutInitWindowPosition (10, 10); 
  glutInit(&argc, argv); 
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); 
  glutCreateWindow (argv[0]); 
  myinit(); 
  glutDisplayFunc(display);  
  glutReshapeFunc(reshape); 
  glutKeyboardFunc(keyboard); // キーボード割込み処理
  glutMainLoop(); 
  return 0; 
} 
