#include <complex.h>
#include <stdio.h>
#include <stdint.h>

#define R 1.5
#define X 10000
#define Y 10000
#define MAX_ITER 1000

double complex grid[X][Y];
uint8_t img[X][Y];

double complex f(double complex z) {
    return z*z + (0.285 + 0.01*I);
//  return z * z + (-0.8 * I);
//  return z * z + (-0.8 + 0.156 * I);
}

int main(int argc, char ** argv) {
  for (int i = 0; i < X; ++i) {
    for (int j = 0; j < Y; ++j) {
//      printf("init %d %d\n", i, j);
      int ci = i-(X/2);
      int cj = j-(Y/2);

      double ai = (ci == 0) ? 0 : ((ci + (X/2)) * (2*R))/X - R;
      double aj = (cj == 0) ? 0 : ((cj + (Y/2)) * (2*R))/Y - R;

      grid[i][j] = ai + aj * I;
    }
  }

  for (int i = 0; i < X; ++i) {
    for (int j = 0; j < Y; ++j) {
      int it = 0;
      double cr = creal(grid[i][j]);
      double ci = cimag(grid[i][j]);
      double cp = R*R;

//      printf("comp %d %d\n", i, j);
      while ((cr * cr + ci * ci) < cp && it < MAX_ITER) {
//        printf("%f\n", (cr * cr + ci * ci));
        grid[i][j] = f(grid[i][j]);
        cr = creal(grid[i][j]);
        ci = cimag(grid[i][j]);
        it++;
      }

      if (it == MAX_ITER) {
        img[i][j] = (uint8_t) 0;
      } else {
        img[i][j] = (uint8_t) (it % 256);
      }
    }
  }

  FILE * f = fopen("test0.dat", "wb");
  fwrite(img, sizeof(uint8_t), X * Y, f);
  fclose(f);
}
