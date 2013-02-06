#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define OUTPUT_FILENAME "torus.obj" // 出力ファイル名
#define PI M_PI

int main(int argc, char** argv) {
  FILE *fout;

  // 出力用ファイルを開く
  fout = fopen(OUTPUT_FILENAME, "w");

  // ファイルを作成できなかった場合のエラー処理
  if(fout == NULL) {
    printf("File can't create\n");
    return 1;
  }

  // 出力するモデルの基本情報
  int div_t   = 30;// tの分割数
  int div_p   = 40;// pの分割数
  double R = 10;// 大半径
  double r = 5;// 小半径

  // 変化させるパラメータ(媒介変数)
  // http://ja.wikipedia.org/wiki/%E3%83%88%E3%83%BC%E3%83%A9%E3%82%B9#.E3.83.89.E3.83.BC.E3.83.8A.E3.83.84.E5.9E.8B
  double t = 0;
  double p = 0;

  // パラメータの変位
  double dt = 2*PI/(div_t);
  double dp = 2*PI/(div_p);

  int i,j;

  // 座標の計算
  for(i=0, t=0; i < div_t; i++, t+=dt) {
    for(j=0, p=0; j < div_p; j++, p+=dp) {
      double x = R * cos(t) + r * cos(p) * cos(t);
      double y = R * sin(t) + r * cos(p) * sin(t);
      double z = r * sin(p);
      fprintf(fout, "v %f %f %f\n", x, y, z); 
    }
  }

  // 面の計算
  for(i=0; i < div_t; i++) {
    for(j=0; j < div_p; j++) {
      int v0 = i*div_p + j;
      int v1 = (i+1)%div_t*div_p + j;
      int v2 = (i+1)%div_t*div_p + (j+1)%div_p;
      fprintf(fout, "f %d %d %d\n", v0+1, v1+1, v2+1);

      //v0 = v0;
      v1 = v2;
      v2 = i*div_p + (j+1)%div_p;
      fprintf(fout, "f %d %d %d\n", v0+1, v1+1, v2+1);
    }
  }

  // 出力用ファイルを閉じる
  fclose(fout);
  return 0;
}
