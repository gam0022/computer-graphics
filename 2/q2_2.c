/*課題2-2　この中で線分を描く関数linei( )と、円を描く関数circlei( )をそれぞれBresenham, 
  Michenerのアルゴリズムで作成して挿入し、プログラムをコンパイル実行すること。レポート
  は、作成した部分（関数単位）のソースプログラムリスト、実行結果の図、考察を入れること。    */

#include <GL/glut.h> 
#include <math.h> 
int width=600, height=600; // 初期ウィンドウの大きさ 

void  ploti(int x, int y) 
{ glBegin(GL_POINTS); glVertex2i(x,y); glEnd(); } 

void  linei(int x1, int y1, int x2, int y2) 
{ 
  // ここに、1点をプロットする関数 ploti(x,y) を使って 
  //  (x1,y1)から(x2,y2)まで直線をプロットする 
  //　Bresenham のアルゴリズムを入れる 

  // init
  int xstart, xend, ystart, yend;

  // mode
  // 0: 普通のプロット
  // 1: xとyをswap
  // 2: y1とy2をswap
  // 3: 1と2の組みわせ
  int mode = 0;

  if(abs(y2 - y1) < abs(x2 - x1)){
    // 傾きが±45°未満
    if(x2>x1) {
      xstart  = x1;
      xend    = x2;
    } else {
      xstart  = x2;
      xend    = x1;
      mode ^= 2;
    }

    if(y2>y1) {
      ystart  = y1;
      yend    = y2;
    } else {
      ystart  = y2;
      yend    = y1;
      mode ^= 2;
    }
  } else {
    // 傾きが±45°以上
    mode |= 1;
    if(y2>y1) {
      xstart  = y1;
      xend    = y2;
    } else {
      xstart  = y2;
      xend    = y1;
      mode ^= 2;
    }

    if(x2>x1) {
      ystart  = x1;
      yend    = x2;
    } else {
      ystart  = x2;
      yend    = x1;
      mode ^= 2;
    }
  }

  int dx = xend - xstart;
  int dy = yend - ystart;
  int d = 2*dy;
  int e = -dx;

  int x = xstart;
  int y = ystart;
  ploti(x,y);

  // main
  while(x < xend) {
    x++;
    e += d;
    if(e>0) {
      y++;
      e -= 2*dx;
    }
    switch(mode) {
      case 0:
        ploti(x,y);
        break;
      case 1:
        ploti(y,x);
        break;
      case 2:
        ploti(x, y1+y2-y);
        break;
      case 3:
        ploti(y, y1+y2-x);
        break;
    }
  }
} 

void  circlei(int x0, int y0, int r) 
{ 
  // ここに、1点をプロットする関数 ploti(x0,y0) を使って 
  //　中心(x0,y0), 半径r の円をプロットする 
  //　Michener のアルゴリズムを入れる  

  // init
  int x = 0;
  int y = r;
  int d = 3 - 2*r;

  ploti(x0,y0+y);
  ploti(x0,y0-y);

  // main
  while(x<y) {
    if (d<0) {
      d += 4*x + 6;
    } else {
      d += 4*(x-y) + 10;
      y--;
    }
    x++;
    ploti(x0+x, y0+y);
    ploti(x0+x, y0-y);
    ploti(x0-x, y0+y);
    ploti(x0-x, y0-y);

    ploti(x0+y, y0+x);
    ploti(x0+y, y0-x);
    ploti(x0-y, y0+x);
    ploti(x0-y, y0-x);
  }
} 

void  display(void) 
{ 
  int i,ix,iy, ix1,iy1; 
  double t; 

  glClear(GL_COLOR_BUFFER_BIT); 
  for( i=0; i<16; i++ ){  
    t = (float)i*22.5*3.1415926545867932/180.;
    ix = (int)(300.*cos(t)) + width/2;
    iy = (int)(300.*sin(t)) + height/2;
    //if(8<=i && i<=12)
    linei( width/2, height/2, ix, iy ); // 中心から放射状に16方向に直線を引く    
  } 
  circlei( width/2,width/2,width/2 ); // ウィンドウ幅の直径の円を描く
  glFlush(); 
} 

void  myinit(void)  
{ 
  glClearColor (1.0, 1.0, 1.0, 0.0);  // 背景色(白) 
  glColor3f(0.,0.,0.);                // 描画色(黒) 
} 

void  reshape(int w, int h) 
{ 
  h = (h == 0) ? 1 : h; 
  glViewport(0, 0, w, h); 
  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity();
  width = w-1; height = h-1; 
  glOrtho( 0.,width, 0.,height, -1., 1. ); 
  glMatrixMode(GL_MODELVIEW); 
} 

void keyboard(unsigned char key, int x, int y) 
{ 
  switch (key) { 
    case 27:  // ESC code  
    case 13:  exit(0);  // RETURN (Enter) code 
    default:break; 
  } 
} 

int  main(int argc, char** argv) 
{ 
  glutInitWindowSize (width, height);  
  glutInitWindowPosition (10, 10); 
  glutInit(&argc, argv); 
  glutInitDisplayMode(GLUT_SINGLE| GLUT_RGB); 
  glutCreateWindow (argv[0]); 
  myinit(); 
  glutDisplayFunc(display);  
  glutReshapeFunc(reshape); 
  glutKeyboardFunc(keyboard); 
  glutMainLoop(); 
  return 0; 
} 
