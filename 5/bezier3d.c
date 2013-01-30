// Bezierオプション課題 (Bezier曲面)
// ３次元空間でマウスによって制御多角形のメッシュを回転できる。
// この制御多角形から,ベジェ曲面を定義する下記のBezierSurf()関数を
// 作成する（そうすれば、曲面がメッシュで表示されるようになっている）
// マウスドラッグによって対象物を回転できる
 
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <GL/glut.h>
 
#define TRACKBALLSIZE  0.8f	// 仮想トラックボールの大きさ
#define	AXIS		1		
 
int	_w,_h;		// 描画ウィンドウ大きさ
float	_aratio;	// 描画ウィンドウ縦横比
float mx0,my0,mx1,my1;	// マウスドラッグ時の始点、終点座標
float	mxsave[]={1.,0.,0.,0., 0.,1.,0.,0., 0.,0.,1.,0., 0.,0.,0.,1.};
float	mx[]={1.,0.,0.,0., 0.,1.,0.,0., 0.,0.,1.,0., 0.,0.,0.,1.};
 
float	mesh[][3]={	// ４×４の制御多角形の頂点群
 	-3.,-2.,-2., -3.,-0.7,-1., -3.,0.7,0., -3.,2.,-1.,
   	-1.,-2.,0., -1.,-0.7,1.3, -1.,0.7,0.8, -1.,2.,0.2,
   1.,-2.,0.5, 1.,-0.7,0.7, 1.,0.7,1.5, 1.,2.,0.9,
   3.,-2.,0., 3.,-0.7,0.3, 3.,0.7,0.6, 3.,2.,0.2 };

// 制御ポリゴンを表示するとき１，しないとき０（スペースキーで変化)
int	dispflag=0;
float	curve[3000][3],curve0[3000][3];	// 曲面上の点群を入れる配列
int	mu=0, mv=0, mu0=0, mv0=0;	// 曲面上の縦方向、横方向の点群の数

int factorial(int n) {
  return (n<=1) ? 1 : n * factorial(n-1);
}

int combination(n,r) {
  return factorial(n) / (factorial(n-r) * factorial(r));
}

float Bezier(int i, int n, float t) {
  return combination(n,i) * pow(t,i) * pow(1-t, n-i);
}

// m×n個の制御点群mesh[][3]から、mu×mv個の曲面上の点群curve[][3]を作成する
//　配列は横方向を優先して1次元に展開して配置している
//　２次元配列の２番目のインデックスはxyz３次元座標値が入る
void	BezierSurf0( float mesh[][3], int m, int n, float curve[][3], int *mu, int *mv )
{
   int	i,j,k,u,v;
 
// ここに制御点群mesh[][3](m行、n列）を元に、ベジェ曲面を定義して、
//　　曲面上の点群を(*mu)×(*mv)個 作成してcurve[][3]に格納すること。
//　　i行目j列目(列の最大値はn列)の点のｘ座標値は２次元配列でcurve[i*n+j][0]で表現できる
//　　現在は単に制御点群を　そのままコピーしてz座標値のみ変更してcurve[][3]に格納している
//　　この下の7行を削除して曲面上の点群を求める関数を作成する。
   int p = 5;
   *mu = p * m;
   *mv = p * n;
   for(u=0; u<*mu; u++) {
     for(v=0; v<*mv; v++) {
       for( k=0; k<3; k++ ) {
         float sum = 0;
         for(i=0; i<=3; i++) {
           for(j=0; j<=3; j++) {
             sum += mesh[i*n+j][k] * Bezier(i,3,(float)u/(*mu-1)) * Bezier(j,3,(float)v/(*mv-1));
           }
         }
         curve[u*(*mv)+v][k] = sum;
       }
     }
   }
}
 
// 配列ｘに入っているｎ個の点を結んで線を描く
void drawLines( float x[][3], int n )
{
   int   i;
   glBegin(GL_LINE_STRIP);
     for( i=0; i<n; i++ ) glVertex3fv(x[i]); 
   glEnd();
}
 
// m行n列の合計mn個の３次元点群の配列wのデータを網で描く
//   配列は横を優先して（行単位で）順次配置している
drawMesh( float w[][3], int m, int n)
{	
   int	i,j;
   for( i=0; i<m; i++ ){
     glBegin(GL_LINE_STRIP);
       for( j=0;j<n;j++ )glVertex3fv(w[i*n+j]);
     glEnd();
   }
   for( j=0; j<n; j++ ){
     glBegin(GL_LINE_STRIP);
       for( i=0;i<m;i++ )glVertex3fv(w[i*n+j]);
     glEnd();
   }
}
 
// 3次元ベクトルの引き算
void vsub(const float *src1, const float *src2, float *dst)
{
   dst[0] = src1[0] - src2[0];
   dst[1] = src1[1] - src2[1];
   dst[2] = src1[2] - src2[2];
}
 
// ベクトル外積計算
void vcross(const float *v1, const float *v2, float *cross)
{
   cross[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
   cross[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
   cross[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
}
 
// 3次元ベクトルの長さを返す
float vlength(const float *v)
{  return (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);}
 
void mxcopy( float s[], float d[] )
{  register i; for( i=0; i<16; i++ ) d[i]=s[i]; }
 
// 行列xに右側から行列aを掛け、結果をｘに入れる
void mxmultiply( float x[], float a[] )
{		
   float w[4][4];    // 作業用行列
   register int   i,j,k;
 
   for( i=0; i<4; i++ ){
     for( j=0; j<4; j++ ){ w[i][j]=0.;
       for( k=0; k<4; k++ ) w[i][j] += x[i*4+k]*a[k*4+j];
     }
   }
   for( i=0; i<4; i++ ){
     for( j=0; j<4; j++ ) x[i*4+j]=w[i][j];
   }
}
 
// 平面上の点(x,y)から仮想トラックボール上の(x,y,z)を求める
static float tb_project_to_sphere(float r, float x, float y)
{	
   float d, z;
   d = (float)sqrt(x*x + y*y);
   if (d < r * 0.70710678118654752440) { // トラックボール上を仮定
     z = (float)sqrt(r*r - d*d);
   } else { // トラックボールを外れた場合、双曲面上を仮定
     z = r*r / (2.*d);
   }
   return z;
}
 
// (u,v,w)方向を回転軸としてtラディアンの回転変換行列xを作る
void axis_to_matrix(float t, float u, float v, float w, float x[])
{
   float c, s, d;
   float uu,uv,uw,vv,vw,ww,c1,us,vs,ws;
 
   if( (d=(float)sqrt(u*u + v*v + w*w)) < 1.e-9 ){
     fprintf(stderr,"no axis defined\n"); u=1.; v=w=0.;
   } else { u /= d; v /= d; w /= d; }
   uu=u*u; uv=u*v; uw=u*w;	vv=v*v; vw=v*w;	ww=w*w;
   c=(float)cos((double)t); s=(float)sin((double)t);
   c1 = 1. - c;	us=u*s; vs=v*s; ws=w*s;
   x[0]=uu+(1.-uu)*c; x[4]=uv*c1-ws; x[8]=uw*c1+vs;
   x[1]=uv*c1+ws; x[5]=vv+(1.-vv)*c; x[9]=vw*c1-us; 
   x[2]=uw*c1-vs; x[6]=vw*c1+us; x[10]=ww+(1.-ww)*c;
   x[3]=x[7]=x[11]=x[12]=x[13]=x[14]=0.;	x[15]=1.;
}
 
// マウスドラッグの始点(p1x,p1y)、終点(p2x,p2y)から回転変換行列mを求める
int trackball(float m[16], float p1x, float p1y, float p2x, float p2y)
{
   float a[3];		// 回転軸
   float phi;		// 回転角（ラディアン単位)
   float p1[3], p2[3], d[3];
   float t;
 
   if( p1x==p2x && p1y==p2y ) return 0;	// return without matrix
   p1[0] = p1x;	p1[1] = p1y;
   p1[2] = tb_project_to_sphere(TRACKBALLSIZE, p1x, p1y);
   p2[0] = p2x;  p2[1] = p2y;
   p2[2] = tb_project_to_sphere(TRACKBALLSIZE, p2x, p2y);
   vcross(p2, p1, a);
   vsub(p1, p2, d);
   t = vlength(d) / (2.0f * TRACKBALLSIZE);
 
   // Avoid problems with out-of-control values
   if (t>1.0)		t = 1.0;
   if (t < -1.0)		t = -1.0;
   phi = 2.0f * (float)asin(t);
   axis_to_matrix(phi, a[0],a[1],a[2], m); 
   return 1;
}
 
void display(void)  // 描画部
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
   glLoadIdentity();
   glTranslatef( 0.,0.,-500. );
   glMultMatrixf( mx );
   glScalef( 50.,50.,50. );
   glCallList( AXIS );		// 予め作成したオブジェクトを表示
 
   glColor3f( 0.,0.5,0. );	// 以降線を濃緑色で描く
   if( dispflag!=2 )drawMesh( mesh, 4, 4 ); // 制御メッシュを描く
   glLineWidth(3.);	// 以降、線幅を3画素とする
   glColor3f( 0.,1.,0. );	// 以降線を緑で描く
   if( dispflag == 0)drawMesh( curve0,mu0,mv0 );	// Bspline curveのメッシュ
   glutSwapBuffers();   
}
 
void  reshape(int w, int h)    // ウィンドウを再描画      
{ 
   h = (h == 0) ? 1 : h;
   glViewport(0, 0, w, h);  
   glMatrixMode(GL_PROJECTION); 
   glLoadIdentity();
   _w = w; _h = h;
   _aratio = (float)w/(float)h;
   gluPerspective( 60., _aratio, 50., 5000. );	// 透視変換
   glMatrixMode(GL_MODELVIEW);
}
 
void myinit(void)    // 初期化
{
   float axis[][3]={1.,0.,0., 0.,0.,0., 0.,1.,0., 0.,0.,0., 0.,0.,1.};
   float charX[][3]={1.2,0.2,0.15, 1.2,-0.2,-0.15, 1.2,0.2,-0.15,
                          1.2,-0.2,0.15};
   float charY[][3]={-0.2,1.5,0., 0.,1.3,0., 0.,1.1,0., 0.,1.3,0., 0.2,1.5,0.};
   float charZ[][3]={-0.15,0.2,1.1, 0.15,0.2,1.1, -0.15,-0.2,1.1, 0.15,-0.2,1.1};
   glClearColor(1.0, 1.0, 1.0, 0.0);	// 背景色を白にする
   glShadeModel(GL_FLAT);   
   glEnable(GL_DEPTH_TEST);	// 奥行き情報で陰線処理をする
 
// 座標軸を描くオブジェクトを作成してID番号をAXISとする
   glNewList(AXIS, GL_COMPILE);
     glColor3f( 0.,0.,0. );	// Black
     glLineWidth(1.);	// 線幅１pixel
     drawLines( axis,5 );	// 座標軸を描く
     drawLines( charX,2 );	// X字
     drawLines( &charX[2],2 );
     drawLines( charY,3 );	// Y字
     drawLines( &charY[3],2 );
     drawLines( charZ,4 );	// Z字
   glEndList();	// オブジェクトの作成終わり
 
   BezierSurf0( mesh,4,4,curve0,&mu0,&mv0 );	// ベジェ曲面上の点群を作成
}
 
void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
     case 27: case 13: break;	// RETURN code */
     case ' ': dispflag++; if( dispflag>2 ) dispflag=0;
                    glutPostRedisplay(); break;
     default:	break;
   }
}
 
// マウス座標をウィンドウの縦方向±１の範囲、
// 横方向±_aratioの範囲の正規化座標系の座標値に変換する
void mousevalue( int x, int y, float *xw, float *yw )
{
   *xw = (2.0f*(float)x/(float)_w - 1.0f)*_aratio;
   *yw = 2.0f*(float)( _h-y-1)/(float)_h - 1.0f;
}
 
void mouse_interrupt(int butn, int state, int x, int y)
{
   if( state == GLUT_DOWN ){  // ボタン押された時
     switch( butn ){
       case GLUT_LEFT_BUTTON: // 左ボタン押された
         mxcopy( mxsave, mx );
         mousevalue( x, y, &mx0, &my0 );
         break;
       case GLUT_RIGHT_BUTTON: // 右ボタン押された
       default: break;
     }
     glutPostRedisplay(); // 再描画
   } 
   else {	// ボタンが放された時
     mxcopy( mx, mxsave );
   }		
}
 
// マウスドラッグによって呼び出される関数
void motion(int x, int y)
{	
   float	wrk[16];
 
   mousevalue( x, y, &mx1, &my1 );
   if( trackball(wrk, mx1, my1, mx0, my0) ){
     mxcopy( mxsave, mx );
     mxmultiply( mx, wrk ); 
     glutPostRedisplay();
   }
}
 
int main(int argc, char** argv) 
{
   glutInitWindowSize(600, 600);
   glutInitWindowPosition(10, 10);
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutCreateWindow(argv[0]);
   myinit();
   glutDisplayFunc(display);	// 描画関数指定
   glutReshapeFunc(reshape);	// 再描画関数
   glutKeyboardFunc(keyboard);	// キーボード割込処理関数指定
   glutMotionFunc(motion);	// マウスドラッグ割込処理関数指定
   glutMouseFunc(mouse_interrupt);	// マウスクリック割込処理関数指定
   glutMainLoop();	// 割り込み待ち
   return 0;
}
