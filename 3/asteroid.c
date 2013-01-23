//
// title:
//  全方位シューティングゲーム
//
// how to play:
//  カーソルキー左右：旋回
//  カーソルキー上：ブースト
//  スペース：ショット

#include <stdio.h> 
#include <stdlib.h> 
#include <GL/glut.h> 
#include <math.h>

#define WIDTH 800
#define HEIGHT 600

#define WIDTH_P  850
#define HEIGHT_P 650

double rand_normal() {
  return (double)rand()/RAND_MAX;
}

double rand_range(double min, double max) {
  return min + (max - min) * rand_normal();
}

typedef struct {
  int type;
  int size;
  int mode;
  int count;
  double x;
  double y;
  double r;
  double vx;
  double vy;
  double vr;
} object;

void obj_init(object obj[], int n, int type) {
  int i;
  object *o;
  for(i=0; i<n; i++) {
    o = &(obj[i]);
    o->type = type;
    o->size = 0;
    o->mode = 0;
    o->count = 0;
  }
}

void obj_move(object *obj) {
  // move
  obj->x += obj->vx;
  obj->y += obj->vy;
  obj->r += obj->vr;

  if (obj->x <= -WIDTH_P) {
    obj->x += 2*WIDTH_P;
  }
  if (obj->x >= WIDTH_P) {
    obj->x -= 2*WIDTH_P;
  }
  if (obj->y <= -HEIGHT_P) {
    obj->y += 2*HEIGHT_P;
  }
  if (obj->y >= HEIGHT_P) {
    obj->y -= 2*HEIGHT_P;
  }
}

void obj_draw(object *obj) {
  obj_move(obj);
  glLoadIdentity();// モデル表示用変換行列に単位行列を入れる
  glTranslatef( obj->x, obj->y,0. );// 並行移動行列を変換行列に掛ける
  glRotatef( obj->r,0.,0.,1. );// (0,0,1)軸の周りにrote度の回転行列を掛ける
  glScalef(obj->size, obj->size, obj->size);// x,y,z軸方向とも50倍に拡大変換行列を掛ける
  glColor3f( 1.,1.,1. );// 描画色を白にする
  glCallList(obj->type);// リストを呼び出す
}

int obj_check(object *obj1, object *obj2) {
  double x = obj1->x - obj2->x;
  double y = obj1->y - obj2->y;
  double r = obj1->size + obj2->size;
  return (x*x + y*y) < r*r;
}

// 度数法からラジアンに変換する
double r_to(int r) {
  return 2.0 * M_PI * r / 360;
}

// ラジアンから座標系に変換
double r_to_x(int r) {
  return sin(r_to(-r));
}
double r_to_y(int r) {
  return cos(r_to(-r));
}

//
// グローバル変数
//

// フラグ
int mode = 0;// 0=>title, 1=>playing
int gameover_cnt = 0;
int mcnt = 0;
int life = 3;
int score = 0;
int remain = 0;
int pause = 0;

// 自機
#define MY 1
object my =  {
  MY, 50, 0, 200,
  0.0, 0.0, 0.0,
  0.0, 0.0, 0.0
};

// 弾
#define TAMA 2
#define tama_n 30
object tama[tama_n] = {0};
int tama_i = 0;
//obj_init(tama, tama_n, TAMA);

// 小惑星
#define ASTEROID 3
#define asteroid_n 300
object asteroid[asteroid_n] = {0};
int asteroid_i = 0;

// ゲームを初期化
void game_init() {
  mcnt = 0;
  life = 3;
  score = 0;
  remain = 0;
  pause = 0;

  obj_init(tama, tama_n, TAMA);
  tama_i = 0;
  obj_init(asteroid, asteroid_n, ASTEROID);
  asteroid_i = 0;
}

// (x,y)位置に文字列string[]を表示 
void strout(int x, int y, char *str) { 
  void *font = GLUT_BITMAP_TIMES_ROMAN_24; 
  glRasterPos2f(x,y); 
  while( *str ) glutBitmapCharacter(font, *str++); 
} 

void list_init(void)
{
  int i;

  glClearColor(0.,0.,0.,0.);    // 消去色
  glClear(GL_COLOR_BUFFER_BIT); // 消去


  //
  // 自機
  //
  glNewList( MY, GL_COMPILE );

  glColor3d(1.0, 1.0, 1.0);
  glLineWidth(2.0);

  glBegin(GL_LINE_STRIP);
  glVertex2d(-0.4, -0.5);
  glVertex2d(0, 0.5);
  glVertex2d(0.4, -0.5);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2d(-0.3, -0.25);
  glVertex2d(0.3, -0.25);
  glEnd();

  glEndList();

  //
  // 弾
  //
  glNewList( TAMA, GL_COMPILE );

  glColor3d(1.0, 1.0, 1.0);
  glLineWidth(2.0);

  glBegin(GL_LINE_STRIP);
  glVertex2d(0, -1);
  glVertex2d(0, 1);
  glEnd();

  glEndList();

  //
  // 小惑星
  //
  glNewList( ASTEROID, GL_COMPILE );

  glColor3d(1.0, 1.0, 1.0);
  glLineWidth(2.0);

  glBegin(GL_LINE_STRIP);
  glVertex2d(0.0, 1.0);
  glVertex2d(1.0, 0.5);
  glVertex2d(0.5, 0.25);
  glVertex2d(1.0, 0.1);
  glVertex2d(1.0, -0.1);
  glVertex2d(0.5, -1.0);
  glVertex2d(-0.5, -1.0);
  glVertex2d(-1.0, -0.5);
  glVertex2d(-1.0, 0.5);
  glVertex2d(0.0, 1.0);
  glEnd();

  glEndList();
}

void reshape(int w, int h)
{
  h = (h == 0) ? 1 : h;        // ウィンドウの高さが0のときは1とする
  glViewport(0, 0, w, h);      // 表示領域はウィンドウ全領域
  glMatrixMode(GL_PROJECTION); // 以降投影用変換行列を設定
  glLoadIdentity();            // 単位行列を変換行列に入れる
  glOrtho( -w,w,-h,h, -1.,1.); // 正射影変換行列を掛ける
  glMatrixMode(GL_MODELVIEW);  // 以降はモデル表示用行列を設定
}

void my_init() {
  my.type = MY;
  my.size = 50;
  my.mode = 1;
  my.count = 200;
  my.x = 0;
  my.y = 0;
  my.r = 0;
  my.vx = 0;
  my.vy = 0;
  my.vr = 0;
}

void new_asteroid(void) {
  object *obj = &(asteroid[asteroid_i]);

  double nx,ny, x,y,r;
  do {
    nx = rand_range(-WIDTH, WIDTH);
    ny = rand_range(-HEIGHT, HEIGHT);
    x = my.x - nx;
    y = my.y - ny;
  } while(x*x + y*y < 100*100);

  obj->type = ASTEROID;
  obj->size = 50;
  obj->mode = 1;
  obj->r = 360 * rand_normal();
  obj->x = nx;
  obj->y = ny;
  obj->vx = rand_normal();
  obj->vy = rand_normal();

  asteroid_i++;
  if(asteroid_i>=asteroid_n) asteroid_i = 0;
  remain++;
}

void break_asteroid(object* obj) {
  int i;

  obj->mode = 0;

  if (obj->size < 25) return;

  for(i=0; i<2; i++) {
    object *new_obj = &(asteroid[asteroid_i]);
    new_obj->type = ASTEROID;
    new_obj->size = obj->size / 2;
    new_obj->mode = 1;
    new_obj->r = 360 * rand_normal();
    new_obj->x = obj->x;
    new_obj->y = obj->y;
    new_obj->vx = rand_normal();
    new_obj->vy = rand_normal();

    asteroid_i++;
    if(asteroid_i>=asteroid_n) asteroid_i = 0;
    remain++;
  }
}

void display(void)
{
  if(pause) return;

  glClear(GL_COLOR_BUFFER_BIT);// 画面消去 

  // 隕石生成
  if(100<=mcnt && mcnt <= 150 && mcnt%10==0) {
    new_asteroid();
  }

  int i, j;
  object *obj, *obj1, *obj2, temp;

  // 隕石
  for(i=0; i<asteroid_n; i++) {
    obj = &(asteroid[i]);
    if(obj->mode == 1) {
      obj_draw(obj);
    }
  }

  // 弾
  for(i=0; i<tama_n; i++) {
    obj = &(tama[i]);
    if(obj->mode == 1) {
      if(obj->count>=0) {
        obj->count --;
      } else {
        obj->mode = 0;
      }
      obj_draw(obj);
    }
  }

  // 自機
  if(my.mode && (my.count==0 || mcnt%2==0)) obj_draw(&my);
  if(my.count>0) {
    my.count--;
  }

  for(i=0; i<asteroid_n; i++) {
    obj1 = &(asteroid[i]);

    if(obj1->mode == 1 && mode) {
      // 小惑星と自機の衝突判定
      if(my.count==0 && obj_check(&my, obj1)) {
        break_asteroid(obj1);
        my.mode = 0;
        life--;
        remain--;
        if (life<0) {
          gameover_cnt = 400;
        } else {
          my_init();
        }
      }

      // 小惑星と弾の衝突判定
      for(j=0; j<tama_n; j++) {
        obj2 = &(tama[j]);
        if(obj2->mode == 1) {
          if(obj_check(obj1, obj2)) {
            break_asteroid(obj1);
            obj2->mode = 0;
            score += 10;
            remain--;
          }
        }
      }
    }
  }

  char str[50];

  // タイトル画面
  if(mode == 0) {
    glLoadIdentity();
    snprintf(str, 50, "ASTEROIDS");
    strout(-110,0, str);

    snprintf(str, 50, "push space to start");
    strout(-160,-100, str);

  } else {

    // score
    glLoadIdentity();
    snprintf(str, 50, "score: %d", score);
    strout(-710,400, str);

    //snprintf(str, 50, "remain: %d", mcnt);
    //strout(-710,300, str);

    // life
    for(i = 0; i<life; i++) {
      temp.type = MY;
      temp.size = 30;
      temp.r = 0;
      temp.x = i*50 - 700;
      temp.y = 500;
      temp.vx = 0;
      temp.vy = 0;
      obj_draw(&temp);
    }

    // gameover
    if (gameover_cnt > 0) {
      glLoadIdentity();
      snprintf(str, 50, "GAME OVER...");
      strout(-130,0, str);
      gameover_cnt --;
      if(gameover_cnt == 0) {
        mode = 0;
      }
    }
  }

  glutSwapBuffers();// 表示画面を切り替えて図形を描く

  mcnt ++;

  if(remain == 0 && mcnt > 105) {
    mcnt = 0;
  }
}

// 矢印キーで割り込み処理
void special_input(int key, int x, int y)
{
  if(mode && my.mode) {
    switch(key){
      case GLUT_KEY_LEFT:  
        my.r += 10.; break;
      case GLUT_KEY_RIGHT: 
        my.r -= 10.; break;
      case GLUT_KEY_UP:
        my.vx += r_to_x(my.r);
        my.vy += r_to_y(my.r);
        break;
      default: return;
    }
  }
  glutPostRedisplay();   // 再描画
}

// 通常キーで割り込み処理
void key_input(unsigned char key, int x, int y)
{
  double vx, vy;

  if(mode) {
    switch(key){
      // ESCキーでポーズ
      case 27: 
        pause ^= 1;
        break;
      case 10: case 13:
        exit(0); //　Enterキーで終了
      case ' ':
        // shot
        if(my.mode) {
          vx = r_to_x(my.r);
          vy = r_to_y(my.r);

          tama[tama_i].type = TAMA;
          tama[tama_i].size = 10;
          tama[tama_i].mode = 1;
          tama[tama_i].count = 100;
          tama[tama_i].r = my.r;
          tama[tama_i].x = my.x + 10.0*vx;
          tama[tama_i].y = my.y + 10.0*vy;
          tama[tama_i].vx = 10.0*vx;
          tama[tama_i].vy = 10.0*vy;

          tama_i++;
          if(tama_i>=tama_n) tama_i = 0;
        }
        break;

      default: break;
    }
  } else {
    switch(key){
      case 27: case 10: case 13:
        exit(0); //　ESC、Enterキーで終了
      case ' ':
        // game start
        game_init();
        my_init();
        mode = 1;
        break;
      default:
        break;
    }
  }
  glutPostRedisplay();
}

int main(int argc, char **argv)
{
  glutInitWindowSize (800, 600); 
  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE); // ダブルバッファモード
  glutCreateWindow(argv[0]);
  list_init();
  glutDisplayFunc(display); // 描画用関数を指定
  glutReshapeFunc(reshape); // 再描画時に呼ばれる関数を指定
  glutIdleFunc(display);	// 常時呼ばれる関数を指定(無し)
  glutSpecialFunc(special_input); //　矢印キーで呼ばれる関数
  glutKeyboardFunc(key_input);    // 通常キー入力による割込関数
  glutMainLoop();
  return 0;
}

