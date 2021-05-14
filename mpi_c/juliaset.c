#include <complex.h>
#include <stdint.h>
#include <mpi.h>

#include "cmdl.h"

double complex f(double complex z);
void distribute();

//double complex f(double complex z) {
//    return z*z + (0.285 + 0.01*I);
//}

int main(int argc, char ** argv) {
  int rank;
  int uprocs;
  int nprocs;
  int x_grid;
  int y_grid;

  parse_cmds(argc, argv, &x_grid, &y_grid, &uprocs);

  MPI_Init(&argc, &argv);  

  // TODO Parse nprocs
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  // TODO create rank
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//  double complex grid = malloc(n * m * sizeof(double complex));
  printf("THREAD\n");
//  init_grid();

  MPI_Finalize();
}
/*
  for (int i = 0; i < X; ++i) {
    for (int j = 0; j < Y; ++j) {
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

      while ((cr * cr + ci * ci) < cp && it < MAX_ITER) {
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
*/
