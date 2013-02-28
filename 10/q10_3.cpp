#include <GL/glut.h>
#include <math.h>   
#include <stdlib.h> 
#include <stdio.h> 
#include <algorithm>
#include <vector>

// 3次元ベクトル
class Vector3d {
  public:
    double x, y, z;
    Vector3d() { x = y = z = 0; }
    Vector3d(double _x, double _y, double _z) { x = _x; y = _y; z = _z; }
    void set(double _x, double _y, double _z) { x = _x; y = _y; z = _z; }
    void set(Vector3d v) {
      x = v.x;
      y = v.y;
      z = v.z;
    }

    // 長さを1に正規化する
    //void normalize() {
    //  double len = length();
    //  x /= len; y /= len; z /= len;
    //}
    Vector3d& normalize() {
      double len = length();
      x /= len; y /= len; z /= len;
      return *this;
    }

    // 長さを返す
    double length() { return sqrt(x * x + y * y + z * z); }

    // s倍する
    //void scale( const double s) { x *= s; y *= s; z *= s;}

    // 演算子のオーバーライド
    inline Vector3d& operator+=( const Vector3d& v ) { x += v.x; y += v.y; z += v.z; return( *this ); }
    inline Vector3d& operator-=( const Vector3d& v ) { x -= v.x; y -= v.y; z -= v.z; return( *this ); }
    inline Vector3d operator+( const Vector3d& v ){ return( Vector3d( x + v.x, y + v.y, z + v.z ) );}
    inline Vector3d operator-( const Vector3d& v ){ return( Vector3d( x - v.x, y - v.y, z - v.z ) );}
    
    Vector3d operator*(double t) {
      return Vector3d(x*t, y*t, z*t);
    }
};

// 質点
class Point {
  public:
    Vector3d f; // 質点に働く力のベクトル
    Vector3d v; // 速度ベクトル
    Vector3d p; // 位置
    bool bFixed; // true: 固定されている false:固定されていない
};

// バネ
class Spring {
  public:
    Point *p0; // 質点
    Point *p1; // 質点
    double restLength; // 自然長

    double length() {
      return (p0->p - p1->p).length();
    }

    Spring(Point *_p0, Point *_p1) {
      p0 = _p0;
      p1 = _p1;
      restLength = length();
    }
};

#define POINT_NUM 40

// 布の定義
class Cloth {
  public:
    Point points[POINT_NUM][POINT_NUM];
    std::vector<Spring*> springs;

    Cloth() {
      // 質点の定義
      for(int y = 0; y < POINT_NUM; y++) {
        for(int x = 0; x < POINT_NUM; x++) {
          points[x][y].bFixed = false;
          points[x][y].p.set(x - POINT_NUM /2, POINT_NUM /2, -y);
        }
      }

      // バネの設定
      for(int y = 0; y < POINT_NUM; y++) {
        for(int x = 0; x < POINT_NUM; x++) {
          // 横方向のバネ
          if(x < POINT_NUM - 1) {				
            springs.push_back(new Spring(&points[x][y], &points[x+1][y]));
          }

          // 縦方向のバネ
          if(y < POINT_NUM -1) {
            springs.push_back(new Spring(&points[x][y], &points[x][y+1]));
          }

          // 右下方向のバネ
          if(x < POINT_NUM - 1 && y < POINT_NUM - 1) {				
            springs.push_back(new Spring(&points[x][y], &points[x+1][y+1]));
          }

          // 左下方向のバネ
          if(x > 0 && y < POINT_NUM - 1) {				
            springs.push_back(new Spring(&points[x][y], &points[x-1][y+1]));
          }
        }
      }

      // 固定点の指定
      //points[0][0].bFixed = true;
      //points[POINT_NUM-1][0].bFixed = true;
    }
};


Cloth *cloth;
double rotateAngleH_deg; // 画面水平方向の回転角度
double rotateAngleV_deg; // 縦方向の回転角度
int preMousePositionX;   // マウスカーソルの位置を記憶しておく変数
int preMousePositionY;   // マウスカーソルの位置を記憶しておく変数
bool bRunning; // アニメーションの実行/停止を切り替えるフラグ

double Ks = 8;   // バネ定数
double Mass = 30; // 質点の質量
double dT = 1; // 時間刻み幅
double Dk = 0.1; // 速度に比例して、逆向きにはたらく抵抗係数
Vector3d gravity(0, -0.002, 0); // 重力(y軸方向の負の向きに働く)

void drawCloth(void) {

  // バネを描画
  glColor3f(0.0f, 0.0f, 0.0f);
  glBegin(GL_LINES);
  for(int i = 0; i < cloth->springs.size(); i++) {
    glVertex3d(cloth->springs[i]->p0->p.x, cloth->springs[i]->p0->p.y, cloth->springs[i]->p0->p.z);
    glVertex3d(cloth->springs[i]->p1->p.x, cloth->springs[i]->p1->p.y, cloth->springs[i]->p1->p.z);
  }
  glEnd();

  // 質点を描画
  glColor3f(1.0f, 0.0f, 0.0f);
  glPointSize(4.0f);
  glBegin(GL_POINTS);
  for(int y = 0; y < POINT_NUM; y++) {
    for(int x = 0; x < POINT_NUM; x++) {
      glVertex3d(cloth->points[x][y].p.x, cloth->points[x][y].p.y, cloth->points[x][y].p.z);
    }
  }
  glEnd();
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_LIGHTING);
  glLoadIdentity();
  glTranslated(0, 0.0, -50);

  glRotated(rotateAngleV_deg, 1.0, 0.0, 0.0);
  glRotated(rotateAngleH_deg, 0.0, 1.0, 0.0);

  drawCloth();

  glFlush();
}

void resize(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
  glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {

  if (key == '\033' || key == 'q') { exit(0);} // ESC または q で終了
  if (key == 'a') { bRunning = !bRunning; }    // a でアニメーションのオンオフ
}

void mouse(int button, int state, int x, int y) {
  switch (button) {
    case GLUT_LEFT_BUTTON:
      preMousePositionX = x;
      preMousePositionY = y;
      break;
    case GLUT_MIDDLE_BUTTON:
      break;
    case GLUT_RIGHT_BUTTON:
      preMousePositionX = x;
      preMousePositionY = y;
      break;
    default:
      break;
  }
}

// 布の形状の更新
void updateCloth(void) {
  // ★次の手順で質点の位置を決定する
  // 1. 質点に働く力を求める
  // 2. 質点の加速度を求める
  // 3. 質点の速度を更新する
  // 4. 質点の位置を更新する

  //1-1. 質点に働く力をリセット
  for(int y = 0; y < POINT_NUM; y++) {
    for(int x = 0; x < POINT_NUM; x++) {
      cloth->points[x][y].f.set(0, 0, 0);
    }
  }

  //1-2. バネの両端の質点に力を働かせる
  // 全てのバネについての処理
  for(int i = 0; i < cloth->springs.size(); i++) {
    // clothオブジェクトの i 番目のバネを取得
    Spring *spring = cloth->springs[i];

    // i 番目のバネの自然長（spring->restLength）と、
		// 現在の長さ（計算で求める）の差分を求める
    double d = spring->restLength - spring->length();

    // 上記の値に、バネ定数 Ks を掛けた値がバネが質点に加える力
    // ただし、力には向きがあるので、大きさだけではなく、方向の情報も必要
    Vector3d Fs = (spring->p0->p - spring->p1->p).normalize() * d * Ks;

    // 両端の質点に対して、力ベクトルを加算する
    // バネの一方の質点に働く力(spring->p0->f)に加算（向きに注意）
    // バネのもう一方の質点に加わる力(spring->p1->f)に加算（向きに注意）
    spring->p0->f += Fs;
    spring->p1->f -= Fs;
  }

  //1-3. 重力、空気抵抗による力を加算する
  // 全ての質点に対する処理
  for(int y = 0; y < POINT_NUM; y++) {
    for(int x = 0; x < POINT_NUM; x++) {
      // cloth->points[x][y].f に、重力、空気抵抗による力を加算する
      Point *p = &(cloth->points[x][y]);
      p->f += gravity * Mass - p->v * Dk;
    }
  }

  // 全ての質点に対する処理
  for(int y = 0; y < POINT_NUM; y++) {
    for(int x = 0; x < POINT_NUM; x++) {
      Point *p = &(cloth->points[x][y]);
      // 2. 質点の加速度ベクトルを計算 (力ベクトル cloth->points[x][y].f を質量で割った値)
      // 3. 質点の速度ベクトル(cloth->points[x][y].v) を更新
      p->v += p->f * (dT/Mass);
      // 4. 質点の位置ベクトル (cloth->points[x][y].p) を更新
      if (!p->bFixed) {
        p->p += p->v * dT;
        // オプション. 球体の内部に入るようなら、強制的に外に移動させる
        Vector3d C(0, 0, -POINT_NUM / 2);// 球体の中心
        Vector3d r = p->p - C;
        if (r.length() < 8) {
          p->p = C + r.normalize() * 8.1;
          p->v.set(0,0,0);
        }
      }
    }
  }
}

void motion(int x, int y) {
  int diffX = x - preMousePositionX;
  int diffY = y - preMousePositionY;

  rotateAngleH_deg += diffX * 0.1;
  rotateAngleV_deg += diffY * 0.1;

  preMousePositionX = x;
  preMousePositionY = y;
  glutPostRedisplay();
}

// 一定時間ごとに実行される
void timer(int value) {
  if(bRunning) {
    updateCloth();
    glutPostRedisplay();
  }

  glutTimerFunc(10 , timer , 0);
}

void init(void) {
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}

int main(int argc, char *argv[]) {

  // Vevtor3d クラスの演算子の使用方法の実験
  Vector3d v0(1,2,3);
  Vector3d v1(1,1,1);
  Vector3d v2 = v1 - v0;
  printf("%lf %lf %lf \n", v2.x, v2.y, v2.z);
  Vector3d v3 = v1 + v0;
  printf("%lf %lf %lf \n", v3.x, v3.y, v3.z);
  v3 += v1;
  printf("%lf %lf %lf \n", v3.x, v3.y, v3.z);
  v3 -= v3;
  printf("%lf %lf %lf \n", v3.x, v3.y, v3.z);


  bRunning = true;
  cloth = new Cloth();

  glutInit(&argc, argv);
  glutInitWindowSize(600,600);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutTimerFunc(10 , timer , 0);

  init();
  glutMainLoop();
  return 0;
}
