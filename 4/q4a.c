/*  
  ３次元座標変換　課題番号４ オプション課題：
  正12面体の展開図（正５角形の集合）を折り曲げて立体を作ること。 
  ただし、このときの折り曲げ角度は63.435度までである。
*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

#define  PENTAGON  1
#define  AXIS	   2
float xrot = 0., yrot = 0.;  // 全体の回転パラメータ
float rotangle = 0.;     // 現在の折り曲げ角度
int   axis = 1;          // 座標軸の表示の有無 

void drawLines( float x[][3], int n )
{	// 配列ｘに入っているｎ個の点を結んで線を描く
  int   i;
  glBegin(GL_LINE_STRIP);
  for( i=0; i<n; i++ )glVertex3fv(x[i]); 
  glEnd();
}

void myinit(void)
{
  int  i;
  float axis[][3]={1.,0.,0., 0.,0.,0., 0.,1.,0., 0.,0.,0., 0.,0.,1.};
  float charX[][3]={1.2,0.2,0.15, 1.2,-0.2,-0.15, 1.2,0.2,-0.15, 1.2,-0.2,0.15};
  float charY[][3]={-0.2,1.5,0., 0.,1.3,0., 0.,1.1,0., 0.,1.3,0., 0.2,1.5,0.};
  float charZ[][3]={-0.15,0.2,1.1, 0.15,0.2,1.1, -0.15,-0.2,1.1, 0.15,-0.2,1.1};
  float light_pos0[] = { -5., 10.0, 50.0, 0. };       // 光源位置座標
  float ambient[]={0.5,0.5,0.5,1.};                   // 環境光の強さ
  float color0[] = {0.7, 0.7, 0.7, 1.};               // 灰色(裏面の色)
  float pentagon[][3] ={0.0, 0.85065, 0, 0.8090162255864723, 0.26286530626504906, 0, 0.49999952486259236, -0.688190306265049, 0, -0.4999995248625922, -0.6881903062650491, 0, -0.8090162255864725, 0.2628653062650489, 0, 0, 0.85065, 0}; // 正五角形の頂点
  float norm[]={0.,0.,-1.};                           // 最初の表面の法線方向

  glColor3f( 0.,0.,0. );	// 線の色を黒にする
  glLineWidth(1.);			  // 線幅１pixel 

  glLightModeli( GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE );  // 両面照光を設定
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);      // 光源位置設定
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);          // 環境光設定
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 画面を背景色に塗りつぶし、ｚバッファを初期化
  glClearColor(1.,1.,1.,0.);                          // 背景色の色設定
  glEnable(GL_DEPTH_TEST);                            // ｚバッファを使った奥行き量判定実施を設定
  glEnable(GL_NORMALIZE);                             // 法線方向をチェックすることを設定
  glEnable(GL_LIGHTING);                              // 光源設定をする
  glEnable(GL_LIGHT0);                                // 光源を点灯させる
  glMaterialfv( GL_BACK, GL_DIFFUSE, color0 );      // 物体の裏面色をcolor0の色に設定

  glNewList( PENTAGON, GL_COMPILE );                    // 正方形の高速表示リストを作る
  glBegin(GL_POLYGON);                                // 四角形をこれから作る
  glNormal3fv( norm );                            // 頂点の法線方向をnormベクトル方向とする
  for( i=0; i<5; i++ )glVertex3fv(pentagon[i]);     // 四角形の頂点座標を設定
  glEnd();                                           // 四角形の作成終わり
  glEndList();                                        // 表示リスト作成の終わり

  glNewList(AXIS, GL_COMPILE);
  drawLines( axis,5 );    // 座標軸を描く 
  drawLines( charX,2 );	  //  X字 
  drawLines( &charX[2],2 );
  drawLines( charY,3 );	  //  Y字
  drawLines( &charY[3],2 );
  drawLines( charZ,4 );	  //  Z字
  glEndList();
}

void reshape(int w, int h)
{
  h = (h == 0) ? 1 : h;    // もしhが0ならば1とする。そうでないならばそのままhとする                   
  glViewport(0, 0, w, h);  // グラフィックウィンドウの全部を表示画面とする
  glMatrixMode(GL_PROJECTION); // これから指定する変換行列は投影変換用行列に右から掛ける
  glLoadIdentity();            // 変換行列に単位行列を入れる
  //　視野角60度,縦横比w/h,視点からの距離1から100の範囲を表示する透視変換行列を掛ける
  gluPerspective(60.0, (GLfloat)w/(GLfloat)h, 1.0, 100.0); 
  glTranslatef( 0.,0.,-50. );  // z方向に－50 の平行移動行列を掛ける
  glMatrixMode(GL_MODELVIEW);  // これから指定する変換行列はモデルビュー変換用行列に掛ける
}

void TranslateAndRotate(double tx, double ty, double tz, double rx, double ry, double rz)
{

  glTranslatef( 0.5 * tx, 0.5 * ty, 0.5 * tz);
  glRotatef(rotangle, rx, ry, rz);
  glTranslatef( -0.5 * tx, -0.5 * ty, -0.5 * tz);
  glTranslatef( tx, ty, tz);
}

void display(void)
{
  float green[] = {0., 1., 0., 1.}; 
  float blue[] = {0., 0., 1., 1.};

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 画面とｚバッファをクリア

  glLoadIdentity();
  glRotatef( xrot,1.,0.,0. );
  glRotatef( yrot,0.,1.,0. );
  glScalef( 5.,5.,5. );

  if( axis==1 ) {         // 座標軸を描く場合
    glDisable(GL_LIGHT0);	// 光源を消す
    glCallList( AXIS );   // 線画を描く
    glEnable(GL_LIGHT0);  // 光源を灯す
  }


  glTranslatef( 0.,0.,-1. );

  glMaterialfv( GL_FRONT, GL_DIFFUSE, blue );//　これから描く物体は青色
  glCallList( PENTAGON );// (A)中心


  glPushMatrix();

    glRotatef(36, 0, 0, -1);
    TranslateAndRotate(0, 1.3764, 0, 1, 0, 0);
    glCallList( PENTAGON );// (A)右上

    glMaterialfv( GL_FRONT, GL_DIFFUSE, green );//　これからは緑色

    glRotatef(36, 0, 0, -1);
    TranslateAndRotate(0, 1.3764, 0, 1, 0, 0);
    glCallList( PENTAGON );// (B)左下

    glRotatef(-36, 0, 0, -1);
    TranslateAndRotate(0, 1.3764, 0, 1, 0, 0);
    glCallList( PENTAGON );// (B)中心

    glPushMatrix();
      glRotatef(-36, 0, 0, -1);
      TranslateAndRotate(0, 1.3764, 0, 1, 0, 0);
      glCallList( PENTAGON );// (B)上
    glPopMatrix();

    glPushMatrix();
      glRotatef(36, 0, 0, -1);
      TranslateAndRotate(0, 1.3764, 0, 1, 0, 0);
      glCallList( PENTAGON );// (B)右上
    glPopMatrix();

    glPushMatrix();
      glRotatef(3*36, 0, 0, -1);
      TranslateAndRotate(0, 1.3764, 0, 1, 0, 0);
      glCallList( PENTAGON );// (B)右下
    glPopMatrix();

    glPushMatrix();
      glRotatef(-3*36, 0, 0, -1);
      TranslateAndRotate(0, 1.3764, 0, 1, 0, 0);
      glCallList( PENTAGON );// (B)左上
    glPopMatrix();

  glPopMatrix();

  glMaterialfv( GL_FRONT, GL_DIFFUSE, blue );//　青色

  glPushMatrix();
    glRotatef(-36, 0, 0, -1);
    TranslateAndRotate(0, 1.3764, 0, 1, 0, 0);
    glCallList( PENTAGON );// (A)左上
  glPopMatrix();

  glPushMatrix();
    glRotatef(36*3, 0, 0, -1);
    TranslateAndRotate(0, 1.3764, 0, 1, 0, 0);
    glCallList( PENTAGON );// (A)右下
  glPopMatrix();

  glPushMatrix();
    glRotatef(180, 0, 0, -1);
    TranslateAndRotate(0, 1.3764, 0, 1, 0, 0);
    glCallList( PENTAGON );// (A)下
  glPopMatrix();

  glPushMatrix();
    glRotatef(36*7, 0, 0, -1);
    TranslateAndRotate(0, 1.3764, 0, 1, 0, 0);
    glCallList( PENTAGON );// (A)左下
  glPopMatrix();

  glutSwapBuffers();        // ダブルバッファを切り替える
}

void special_input(int key, int x, int y)
{
  switch(key){
    case GLUT_KEY_LEFT:
      yrot -= 2.; if( yrot<0. ) y += 360.; break;
    case GLUT_KEY_RIGHT:
      yrot += 2.; if( yrot>360. ) y-= 360.;break;
    case GLUT_KEY_UP:
      xrot -= 2.; if( xrot <0. ) x += 360.; break;
    case GLUT_KEY_DOWN:
      xrot += 2.; if( xrot > 360. ) x-=360.; break;
    default: return;
  }
  glutPostRedisplay();
}

void key_input(unsigned char key, int x, int y)
{
  switch(key){
    case 'a': axis = 1 - axis; glutPostRedisplay(); break;
    case 'f': rotangle +=1.;  // 折りたたむ
              if( rotangle>63.435 ) rotangle=63.435; glutPostRedisplay(); break;
    case 'u': rotangle -=1.;// 展開する、広げる
              if( rotangle<0. ) rotangle=0.; glutPostRedisplay(); break;
    case 27:  // ESC キーコード
    case 13:  // CR コード
    case 10:  exit(0); // LF コード Enter, RETURN キー
    default:   return;
  }
}

int main(int argc, char **argv)
{
  glutInitWindowSize (800, 600); 
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutCreateWindow(argv[0]);
  myinit();
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutMouseFunc(NULL);
  glutSpecialFunc(special_input);
  glutKeyboardFunc(key_input);
  glutMainLoop();
  return 0;
}
