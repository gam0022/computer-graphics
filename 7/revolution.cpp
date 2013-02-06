#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define OUTPUT_FILENAME "revolution.obj" // 出力ファイル名
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

  int i,j;

  // 出力するモデルの基本情報
  double height = 5;
  int div_z = 20;
  int div_phi = 40;

  double r[200];
  for(i=0; i<=div_z; i++) {
    //r[i] = 1 + cos(PI*i/div_z*2);
    r[i] = 2 + cos(PI*pow(i,1.7)*0.01);
  }

  // 変化させるパラメータ
  double z = -0.5 * height;
  double phi;

  // パラメータの変位
  double dz   = height/div_z;
  double dphi = 2*PI/div_phi;

  // 座標の計算
  for(i=0; i <= div_z; i++, z+=dz) {
    for(j=0, phi=0; j < div_phi; j++, phi+=dphi) {
      double x = r[i] * sin(phi);
      double y = r[i] * cos(phi);
      fprintf(fout, "v %f %f %f\n", x, -z, y); 
    }
  }

  // 面の計算
  for(i=0; i < div_z; i++) {
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
