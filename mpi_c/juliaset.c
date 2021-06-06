/*
 * File:  main.c
 * --------------------
 *   distribute ICs -> calculate julia set on individual processors -> collect result from all processors
 */

#include <complex.h>
#include <stdint.h>
#include <stdio.h>
#include <mpi.h>

#include "cmdl.h"
#include "control.h"
#include "grid.h"

// Global data ref. control.h
control ctrl;

/*
 * Function: main
 * ----------------------------
 *   Calculate julia set and output image based input arguments
 *
 *   argc: number of arguments
 *   argv: arguments
 *
 *   returns: final program state
 */
int main(int argc, char ** argv) {
  // Current processor ID and maximum processor
  int rank;
  int max_nprocs;

  // Initialize the main grid -- this will be used for recovering the result
  MPI_Init(&argc, &argv);  

  // Capture cmd line arguments and store in control ctrl
  parse_cmds(argc, argv);

  MPI_Comm_size(MPI_COMM_WORLD, &max_nprocs);

  // Make sure user requested proces does not exceed max allocation
  if (ctrl.tprocs > max_nprocs) {
    printf("Exceeded maximum processor count");
    exit(-1);
  }
  
  // Store ID of processors in rank
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Allocate a subgrid and subimg for each processor
  double complex * pad = pad_alloc(rank);
  uint8_t * img_seg = img_alloc(rank);

  // Let just thread 0 perform the initialization
  double complex * grid = init(rank);
  uint8_t * img = (rank == 0) ? (uint8_t *) malloc(ctrl.x_grid*ctrl.y_grid*sizeof(uint8_t)) : NULL;

  // Send initial grid to all processors
  distribute_ic(grid, rank, pad);

  // Calculate the julia set individually on all processors
  simulate(pad, img_seg, rank);

  // Collect the image results
  collate(img, img_seg, rank);

  free(pad);
  free(img_seg);

  // Only processor 0 will serialize the image result
  if (rank == 0) {
    FILE * f = fopen("test.dat", "wb");
    fwrite(img, sizeof(uint8_t), ctrl.x_grid * ctrl.y_grid, f);
    fclose(f);

    free(img); 
    free(grid); 
  }

  MPI_Finalize();

  return 0;
}
