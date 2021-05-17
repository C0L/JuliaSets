#include <complex.h>
#include <stdint.h>
#include <stdio.h>
#include <mpi.h>

#include "cmdl.h"
#include "control.h"
#include "grid.h"

control ctrl;

int main(int argc, char ** argv) {
  int rank;
  int max_nprocs;

  parse_cmds(argc, argv);
  printf("%d\n", ctrl.y_grid);
  printf("%d\n", ctrl.x_grid);
  printf("%d\n", ctrl.nprocs);
  printf("%d\n", ctrl.iters);

  // Initialize the main grid -- this will be used for recovering the result
  double complex * grid = (double complex *) malloc(ctrl.y_grid * ctrl.x_grid * sizeof(double complex));
  uint8_t * img = (uint8_t *) malloc(ctrl.y_grid * ctrl.x_grid * sizeof(uint8_t));

  MPI_Init(&argc, &argv);  
  // TODO Parse nprocs
  MPI_Comm_size(MPI_COMM_WORLD, &max_nprocs);

  if (ctrl.tprocs > max_nprocs) {
    printf("Exceeded maximum processor count");
    exit(-1);
  }

  // TODO create rank
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  double complex * pad = pad_alloc(rank);
  uint8_t * img_seg = img_alloc(rank);

  // Let just thread 0 perform the initialization
  init(grid, rank);
  distribute_ic(grid, rank, pad);

  MPI_Finalize();
}

/*
  FILE * f = fopen("test0.dat", "wb");
  fwrite(img, sizeof(uint8_t), X * Y, f);
  fclose(f);
*/
