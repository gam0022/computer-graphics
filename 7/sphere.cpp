#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define OUTPUT_FILENAME "sphere.obj" // 出力ファイル名
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
  int div_phi   = 30;// Φ方向の分割数
  int div_theta = 20;// θ方向の分割数
  double r = 10;// 半径

  // 変化させるパラメータ
  // http://ja.wikipedia.org/wiki/%E6%A5%B5%E5%BA%A7%E6%A8%99%E7%B3%BB#.E7.90.83.E5.BA.A7.E6.A8.99_.28Spherical_Polar_Coordinates.29
  double phi = 0;
  double theta = 0;

  // パラメータの変位
  double dtheta = PI/(div_theta);
  double dphi   = 2*PI/(div_phi);

  int i,j;

  // 座標の計算
  for(i=0, theta=0; i <= div_theta; i++, theta+=dtheta) {
    for(j=0, phi=0; j < div_phi; j++, phi+=dphi) {
      double x = r * sin(theta) * cos(phi);
      double y = r * sin(theta) * sin(phi);
      double z = r * cos(theta);
      fprintf(fout, "v %f %f %f\n", x, y, z); 
    }
  }

  // 面の計算
  for(i=0; i < div_theta; i++) {
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
