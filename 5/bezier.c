//  マウスで点を続けて入力すると、それらを結ぶ折れ線が表示され、
//  さらに、各線分の中点を結ぶ点群を求めて、それらを結ぶ折れ線を描いている
//　中点を求める代わりに、ベジェ曲線上の点群を求めて、
//　それらを同じ配列に入れ替えれば、ベジェ曲線が描けるはず

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#define MAX     100
#define partition 10

float pt[MAX*100][2]; // マウスからの点の座標を格納 
int   np = 0;     // マウスの点の個数 

void drawPoints( float x[][2], int n )
{		/* 配列Xのn個の点をプロットする */
  int i;
  if( n == 0 )return;
  glBegin(GL_POINTS);
  for( i=0; i<n; i++ ) glVertex2fv( x[i] );
  glEnd();
}

void drawLines( float x[][2], int n )
{		// 配列xのn個の点を結んで折れ線を描く
  int i;
  glBegin(GL_LINE_STRIP);
  for( i=0; i<n; i++ ) glVertex2fv( x[i] );
  glEnd();
}

// 点列からBezier曲線を太く描く 
void display(void)
{
  float c[2000][2];  // 曲線上の点列を格納する配列
  int  i,j, j2, k;

  glClear(GL_COLOR_BUFFER_BIT); // 画面消去
  drawPoints( pt,np );
  drawLines( pt,np );
  // 以下の5行を消去して、Bezier曲線上の点列を生成して配列 c[][2]に格納すること
  if( np < 3 ){ glFlush(); return; }
  //for( i=0,j=0; i<np-1; i++,j++ ){
  //  c[j][0] = (pt[i][0] + pt[i+1][0])/2.;   // x 座標値
  //  c[j][1] = (pt[i][1] + pt[i+1][1])/2.;   // y 座標値
  //}
  for(i=0,j=0; i<np-3; i+=3) {
    for(j2=0; j2<=partition; j++,j2++) {
      double t = (double)j2/partition;
      for( k=0; k<2; k++) {
        c[j][k] = (pow(1-t,3) * pt[i][k]) + (3 * t * pow(1-t,2) * pt[i+1][k]) 
          + (3 * pow(t,2) * (1-t) * pt[i+2][k]) + (pow(t,3) * pt[i+3][k]);
      }
    }
  }
  glLineWidth(3.); // 線の太さを3pixels にする
  drawLines( c,j );// 配列cの中のj個の点を結んで折れ線を描く
  glLineWidth(1.); // 線の太さを1に戻す
  glFlush();       // 画面に出力
}
void mouse_interrupt(int butn, int state, int x, int y)
{
  if( state == GLUT_DOWN ){	// ボタン押された時
    switch( butn ){
      case GLUT_LEFT_BUTTON: // 左ボタン押された
        if( np < MAX-1 ){		// バッファを越えなければマウスの点を登録
          pt[np][0]=x; pt[np][1]=y; np++;
        } break;
      case GLUT_RIGHT_BUTTON: // 右ボタン押されたとき
        if( np > 0 ) np--;	break;	// 点が定義されていれば、最後の点を消去
      default: break;
    }
    glutPostRedisplay(); // 再描画する
  } 
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
    case 27:	          // ESC code
    case 13:	exit(0);	// RETURN code 
    default:	break;
  }
}

void  reshape(int w, int h)
{
  h = (h == 0) ? 1 : h;
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // 画面の座標系指定 0 < x < w,  0 < y < h 
  glOrtho(0.,(float)w,(float)h,0.,-10.,10. );
  glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
  glutInitWindowSize(600, 600);
  glutInitWindowPosition(10, 10);
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutCreateWindow(argv[0]);
  glClearColor(1.,1.,1.,0.); // 初期画面色 
  glColor3f( 0., 0., 0. ); // 描く線の色 
  glPointSize( 7. );       // 描く点の大きさ 
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse_interrupt);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
