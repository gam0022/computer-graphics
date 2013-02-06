// 
// 曲線を回転させてできる立体を作成するプログラム
//  
// 操作方法
//   左クリック:
//     曲線を描く。
//   右クリック:
//     直前の線を消す。
//   エンター:
//     OBJ ファイルを出力する。

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

#define MAX     100

#define OUTPUT_FILENAME "revolution2.obj" // 出力ファイル名
#define PI M_PI


float pt[MAX*100][2]; // マウスからの点の座標を格納 
int   np = 0;     // マウスの点の個数 

int out_obj() {
  FILE *fout;

  // 出力用ファイルを開く
  fout = fopen(OUTPUT_FILENAME, "w");

  // ファイルを作成できなかった場合のエラー処理
  if(fout == NULL) {
    printf("File can't create\n");
    return 1;
  }

  int i,j;

  // 出力するモデルの基本情報
  int div_phi = 40;

  // 変化させるパラメータ
  double phi;

  // パラメータの変位
  double dphi = 2*PI/div_phi;

  // 座標の計算
  for(i=0; i < np; i++) {
    for(j=0, phi=0; j < div_phi; j++, phi+=dphi) {
      double z = pt[i][1] - 300;
      double r = pt[i][0] - 300;
      double x = r * sin(phi);
      double y = r * cos(phi);
      fprintf(fout, "v %f %f %f\n", x, -z, y); 
    }
  }

  // 面の計算
  for(i=0; i < np; i++) {
    for(j=0; j < div_phi; j++) {
      int v0 = i*div_phi + j;
      int v1 = (i+1)*div_phi + j;
      int v2 = (i+1)*div_phi + (j+1)%div_phi;
      fprintf(fout, "f %d %d %d\n", v0+1, v1+1, v2+1);

      //v0 = v0;
      v1 = v2;
      v2 = i*div_phi + (j+1)%div_phi;
      fprintf(fout, "f %d %d %d\n", v0+1, v1+1, v2+1);
    }
  }

  // 出力用ファイルを閉じる
  fclose(fout);
  return 0;
}

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

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT); // 画面消去

  // ガイドライン
  float g[2][2] = {300,0,300,600};
  glLineWidth(3.);
  glColor3f(1,0,0);
  drawLines( g,2 );

  glLineWidth(3.); // 線の太さを3pixels にする
  glColor3f(0,0,0);
  drawPoints( pt,np );
  drawLines( pt,np );
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
    case 27:	exit(0);  // ESC code
    case 13:	out_obj();// RETURN code 
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
