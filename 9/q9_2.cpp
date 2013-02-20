#include <GL/glut.h>
#include <math.h>   
#include <stdlib.h> 
#include <stdio.h> 
#include <algorithm>

// 3次元ベクトル
class Vector3d {
  public:
    double x, y, z;

    Vector3d() {
      x = y = z = 0;
    }

    Vector3d(double _x, double _y, double _z) {
      x = _x;
      y = _y;
      z = _z;
    }

    // 長さを1に正規化する
    void normalize() {
      double len = length();
      x /= len;
      y /= len;
      z /= len;
    }

    // 長さを返す
    double length() {
      return sqrt(x * x + y * y + z * z);
    }

    // 値を設定する
    void set(double _x, double _y, double _z) {
      x = _x;
      y = _y;
      z = _z;	
    }

    void set(Vector3d &vec) {
      set(vec.x, vec.y, vec.z);
    }

    // 演算子をオーバーロードする
    Vector3d operator*(double t) {
      return Vector3d(x*t, y*t, z*t);
    }
    Vector3d operator-(Vector3d v) {
      return Vector3d(x - v.x, y - v.y, z - v.z);
    }
    Vector3d operator+(Vector3d v) {
      return Vector3d(x + v.x, y + v.y, z + v.z);
    }
    Vector3d operator-() {
      return Vector3d(-x, -y, -z);
    }
};

// 球体
class Sphere {
  public:
    Vector3d center; // 中心座標
    double radius;   // 半径
    double cR, cG, cB;  // Red, Green, Blue 値 0.0～1.0

    Sphere(double x, double y, double z, double r, 
        double cr, double cg, double cb) {
      center.x = x;
      center.y = y;
      center.z = z;
      radius = r;
      cR = cr;
      cG = cg;
      cB = cb;
    }

    // 点pを通り、v方向のRayとの交わりを判定する。
    // 交点が p+tv として表せる場合の t の値を返す。交わらない場合は-1を返す
    double getIntersec(Vector3d &p, Vector3d &v) { 
      // A*t^2 + B*t + C = 0 の形で表す
      double A = v.x * v.x + v.y * v.y + v.z * v.z;
      double B = 2.0 * (p.x * v.x - v.x * center.x +
          p.y * v.y - v.y * center.y + 
          p.z * v.z- v.z * center.z);
      double C = p.x * p.x - 2 * p.x * center.x + center.x * center.x + 
        p.y * p.y - 2 * p.y * center.y + center.y * center.y + 
        p.z * p.z - 2 * p.z * center.z + center.z * center.z -
        radius * radius;
      double D = B * B - 4 * A * C; // 判別式

      if (D >= 0) { // 交わる
        double t1 = (-B - sqrt(D)) / (2.0 * A);
        double t2 = (-B + sqrt(D)) / (2.0 * A);
        return t1 < t2 ? t1 : t2; // getPixcelColor小さいほうのtの値を返す
      } else { // 交わらない
        return -1.0;
      }
    }
};

// 板。xz平面に平行な面とする
class Board {
  public:
    double y; // y座標値

    Board(double _y) {
      y = _y;
    }

    // 点pを通り、v方向のRayとの交わりを判定する。
    // 交点が p+tv として表せる場合の t の値を返す。交わらない場合は負の値を返す
    double getIntersec(Vector3d &p, Vector3d &v) { 
      if(fabs(v.y) < 1.0e-10) return -1; // 水平なRayは交わらない

      double t = -1;
      // ★ここで t の値を計算する
      t = (y - p.y) / v.y;
      // ★ただしz座標が-3000より小さいなら交わらないものとする
      double z = p.z + v.z * t;
      if(z < -3000) {
        t = -1;
      }

      return t;
    }

    // x と y の値から床の色を返す（格子模様になるように）
    Vector3d getColorVec(double x, double z) {
      // ★ x, z の値によって(1.0, 1.0, 0.7)または(0.6, 0.6, 0.6)のどちらかの色を返すようにする
      if(((int)x/100 + (int)z/100 + (x<0)) % 2) {
        return Vector3d(1.0, 1.0, 0.7);
      }else{
        return Vector3d(0.6, 0.6, 0.6);
      }
    }
};

int halfWidth;    // 描画領域の横幅/2
int halfHeight;   // 描画領域の縦幅/2
double screen_z = -1000;  // 投影面のz座標
double Kd = 0.8;  // 拡散反射定数
double Ks = 0.8;  // 鏡面反射定数
double Ns = 3;
double Iin = 0.5; // 入射光の強さ 
double Ia  = 0.5; // 環境光
//double Iin = 1.0; // 入射光の強さ 
//double Ia  = 0.2; // 環境光
Vector3d viewPosition(0, 0, 0); // 原点=視点
Vector3d lightDirection(-2, -4, -2); // 入射光の進行方向


// レンダリングする球体
Sphere sphere(0.0, 0.0, -1500, // 中心座標
    150.0,           // 半径
    0.1, 0.7, 0.7);  // RGB値

// 球体の置かれている床
Board board(-150); // y座標値を -150 にする。（球と接するようにする）

// 2つのベクトルの成す角のcos値を計算する
double getCos(Vector3d &v0, Vector3d &v1) {
  return (v0.x * v1.x + v0.y * v1.y + v0.z * v1.z ) / (v0.length() * v1.length());
}

double pow2(double a, double b) {
  return a>0 ? pow(a,b) : 0;
}

// x, y で指定されたスクリーン座標での色(RGB)を colorVec のxyzの値に格納する
void getPixcelColor(double x, double y, Vector3d &colorVec) {
  // 原点からスクリーン上のピクセルへ飛ばすレイの方向
  Vector3d ray(x - viewPosition.x, y - viewPosition.y, screen_z - viewPosition.z); 

  ray.normalize(); // レイの長さの正規化

  // レイを飛ばして球と交差するか求める
  double t_sphere = sphere.getIntersec(viewPosition, ray);

  if(t_sphere > 0) { // 球との交点がある
    // ★前回の課題を参考に、球体の表面の色を計算で求め、colorVecに設定する

    double Is = 0; // 鏡面反射光
    double Id = 0; // 拡散反射光

    //
    // 拡散反射光を計算する
    //

    // 球と視点ベクトルの交点座標 P
    Vector3d P = ray * t_sphere;
    // 球の中心座標 C
    Vector3d C = sphere.center;
    // 法線ベクトル  N = P - C
    Vector3d N = P - C;
    // Nを正規化する
    N.normalize();
    // Lambertの反射(拡散反射光)
    Id = -Iin * Kd * getCos(N, lightDirection);
    Id = Id > 0 ? Id : 0;

    //
    // 鏡面反射光を計算する
    //

    // 反射光 R=2(L・N)N-L
    Vector3d R = N * getCos(lightDirection, N) * 2 - lightDirection;
    // Phongの反射(鏡面反射光)
    Is = Iin * Ks * pow2(getCos(R, ray), Ns);
    Is = Is > 0 ? Is : 0;

    double I = Id + Is + Ia;
    double r = std::min(I * sphere.cR, 1.0);
    double g = std::min(I * sphere.cG, 1.0);
    double b = std::min(I * sphere.cB, 1.0);
    colorVec.set(r, g, b);
    return;
  } 

  // レイを飛ばして床と交差するか求める
  double t_board = board.getIntersec(viewPosition, ray);

  if(t_board > 0) { // 床との交点がある
    // ★床の表面の色を設定する
    Vector3d P = ray * t_board;
    Vector3d color =  board.getColorVec(P.x, P.z);
    // ★球の影になる場合は、RGBの値をそれぞれ0.5倍する
    Vector3d ray2 = -lightDirection;
    double t = sphere.getIntersec(P, ray2);
    if(t>0) {
      color = color * 0.5;
    }
    double r = color.x;
    double g = color.y;
    double b = color.z;
    colorVec.set(r, g, b);
    return;
  }

  // 何とも交差しない
  colorVec.set(0, 0, 0); // 背景色（黒）を設定する
}

// 描画を行う
void display(void) {

  glClear(GL_COLOR_BUFFER_BIT); // 描画内容のクリア

  // ピクセル単位で描画色を決定するループ処理
  for(int y = (-halfHeight); y <= halfHeight; y++ ) {
    for(int x = (-halfWidth); x <= halfWidth; x++ ) {

      Vector3d colorVec(0,0,0);
      Vector3d colorVecTmp;

      // x, y 座標の色を取得する
      for(int xi = 0; xi<3; xi++) {
        for(int yi = 0; yi<3; yi++) {
          getPixcelColor(x + xi * 1.0/3, y + yi * 1.0/3, colorVecTmp);
          colorVec = colorVec + colorVecTmp * (1.0/9);
        }
      }

      //取得した色で、描画色を設定する
      glColor3d(colorVec.x, colorVec.y, colorVec.z); 

      // (x, y) の画素を描画
      glBegin(GL_POINTS); 
      glVertex2i( x, y ); 
      glEnd( ); 
    }
  }
  glFlush();
}

void resizeWindow(int w, int h) {
  h = (h == 0) ? 1 : h;
  glViewport(0, 0, w, h);
  halfWidth = w/2;
  halfHeight = h/2;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // ウィンドウ内の座標系設定
  glOrtho( -halfWidth, halfWidth, -halfHeight, halfHeight, 0.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27: exit(0);  /* ESC code */
  } 
  glutPostRedisplay();
}

int main(int argc, char** argv) {
  lightDirection.normalize();

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(400,400);
  glutInitWindowPosition(180,10);
  glutCreateWindow(argv[0]);
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glShadeModel(GL_FLAT);

  glutDisplayFunc(display);
  glutReshapeFunc(resizeWindow);
  glutKeyboardFunc(keyboard);
  glutMainLoop();

  return 0;
}

