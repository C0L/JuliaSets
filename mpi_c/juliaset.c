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

  // Initialize the main grid -- this will be used for recovering the result
  MPI_Init(&argc, &argv);  

  parse_cmds(argc, argv);
  printf("%d\n", ctrl.y_grid);
  printf("%d\n", ctrl.x_grid);
  printf("%d\n", ctrl.nprocs);
  printf("%d\n", ctrl.iters);

  // TODO Parse nprocs
  MPI_Comm_size(MPI_COMM_WORLD, &max_nprocs);
  printf("Init\n");

  if (ctrl.tprocs > max_nprocs) {
    printf("Exceeded maximum processor count");
    exit(-1);
  }

  // TODO create rank
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  printf("Ranked\n");
  double complex * pad = pad_alloc(rank);
  uint8_t * img_seg = img_alloc(rank);

  // Let just thread 0 perform the initialization
  printf("Init %d\n", rank);
  double complex * grid = init(rank);
  uint8_t * img = (rank == 0) ? (uint8_t *) malloc(ctrl.x_grid*ctrl.y_grid*sizeof(uint8_t)) : NULL;

  printf("Distribute IC %d\n", rank);
  distribute_ic(grid, rank, pad);
  printf("Simulate %d\n", rank);
  simulate(pad, img_seg, rank);
  printf("Collate %d\n", rank);
  collate(img, img_seg, rank);
  printf("Complete %d\n", rank);

  free(pad);
  free(img_seg);

  if (rank == 0) {
    FILE * f = fopen("test0.dat", "wb");
    fwrite(img, sizeof(uint8_t), ctrl.x_grid * ctrl.y_grid, f);
    fclose(f);

    free(img); 
    free(grid); 
  }

  MPI_Finalize();
}
