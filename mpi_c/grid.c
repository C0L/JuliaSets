/*
 * File:  grid.h
 * --------------------
 *   defines actions to be taken on the simulation grid
 */

#include "grid.h"
#include <stdio.h>

/*
 * Function: init
 * ----------------------------
 *   Process 0 initilizes the complex grid based on the size of the complex grid R
 *
 *   rank: the processor ID
 *
 *   returns: pointer to the initialized grid (only for processor 0), NULL otherwise
 */
double complex * init(int rank) {
  // Let only processor 0 take this path
  if (rank == 0) {
    // Instantiate the IC grid
    double complex * grid = (double complex *) malloc(ctrl.y_grid * ctrl.x_grid * sizeof(double complex));

    // Map [-R,R],[-R,R] region of complex plane to the grid
    for (int i = 0; i < ctrl.x_grid; ++i) {
      for (int j = 0; j < ctrl.y_grid; ++j) {
        int ci = i-(ctrl.x_grid/2);
        int cj = j-(ctrl.y_grid/2);

        // Center region of julia set at 0,0
        double ai = (ci == 0) ? 0 : ((ci + (ctrl.x_grid/2)) * (2*ctrl.region))/ctrl.x_grid - ctrl.region;
        double aj = (cj == 0) ? 0 : ((cj + (ctrl.y_grid/2)) * (2*ctrl.region))/ctrl.y_grid - ctrl.region;

        *(grid + i + ctrl.x_grid * j) = ai + aj * I;
      }
    }
    return grid;
  } else {
    return NULL;
  }
}

/*
 * Function: pad_alloc
 * ----------------------------
 *   Allocate a 'pad' or the computational region of a single processor
 *
 *   rank: the processor ID
 *
 *   returns: pointer to the initialized pad
 */
double complex * pad_alloc(int rank) {
  // Calculate the size of the grid based on the rank
  int ideal_x = ctrl.x_grid / ctrl.nprocs;
  int ideal_y = ctrl.y_grid / ctrl.nprocs;

  int diff_x = (ctrl.nprocs * ideal_x) - ctrl.x_grid;
  int diff_y = (ctrl.nprocs * ideal_y) - ctrl.y_grid;

  if (rank < diff_x) {
    ideal_x++; 
  }

  if (rank % ctrl.nprocs < diff_y) {
    ideal_y++; 
  }
  
  // Allocate a 'sub-grid' for each processor
  double complex * pad  = (complex double *) malloc(ideal_x * ideal_y * sizeof(double complex));

  return pad;
}

/*
 * Function: img_alloc
 * ----------------------------
 *   Allocate a 'img' subsection for each processor
 *
 *   rank: the processor ID
 *
 *   returns: pointer to the initialized pad
 */
uint8_t * img_alloc(int rank) {
  // Calculate the size of the img subsection
  int ideal_x = ctrl.x_grid / ctrl.nprocs;
  int ideal_y = ctrl.y_grid / ctrl.nprocs;

  int diff_x = (ctrl.nprocs * ideal_x) - ctrl.x_grid;
  int diff_y = (ctrl.nprocs * ideal_y) - ctrl.y_grid;

  if (rank < diff_x) {
    ideal_x++; 
  }

  if (rank % ctrl.nprocs < diff_y) {
    ideal_y++; 
  }

  // Allocate a image subsection
  uint8_t * pad  = (uint8_t*) malloc(ideal_x * ideal_y * sizeof(uint8_t));

  return pad;
}

/*
 * Function: distribute_ic
 * ----------------------------
 *   Processor 0 sends initial conditions to other N-1 processors
 *
 *   grid: the initial conditions
 *   rank: the ID of the processor
 *   pad: the processors subsection of the entire grid it will perform the computation on
 *
 *   returns: void
 */
void distribute_ic(complex double * grid, int rank, complex double * pad) {
  // If the processor is 0 it needs to distribute the IC to the other processors
  if (rank == 0) {
    // Iterate through every processor and send subsection of grid to pad of other processor
    for (int p = 0; p < ctrl.tprocs; ++p) {
      // Calculate indexes into main grid
      int ideal_x = ctrl.x_grid / ctrl.nprocs;
      int ideal_y = ctrl.y_grid / ctrl.nprocs;

      int diff_x = (ctrl.nprocs * ideal_x) - ctrl.x_grid;
      int diff_y = (ctrl.nprocs * ideal_y) - ctrl.y_grid;

      if (p < diff_x) {
        ideal_x++; 
      } 

      if (p % ctrl.nprocs < diff_y) {
        ideal_y++; 
      }

      // Create block that we will fill and transfer to the pad of another processor
      complex double * transfer_block = (complex double *) malloc(ideal_x * ideal_y * sizeof(complex double));

      // TODO More sophisicated division needed here for non-evenly divisble cases
      //int global_x = (p < diff_x ? p * ideal_x : p * (ideal_x + 1)) + (p >= diff_x ? p * ideal_x : 0); 
      //int global_y = ((p % ctrl.nprocs) < diff_y ? p * ideal_y : (p % ctrl.nprocs) * (ideal_y + 1)) + (p >= diff_y ? (p % ctrl.nprocs) * ideal_y : 0);   
//      int global_x = ((p % ctrl.nprocs) * ideal_x);
//      int global_y = ((p % ctrl.nprocs) * ideal_y);
      //(p < diff_x ? p * ideal_x : p * (ideal_x + 1)) + (p >= diff_x ? p * ideal_x : 0); 
      //int global_y = ((p % ctrl.nprocs) < diff_y ? p * ideal_y : (p % ctrl.nprocs) * (ideal_y + 1)) + (p >= diff_y ? (p % ctrl.nprocs) * ideal_y : 0);   

      int global_x = ((p % ctrl.nprocs) * ideal_x);
      int global_y = ((p / ctrl.nprocs) * ideal_y);

      // Copy processors section of grid into the transfer grid
      for (int i = 0; i < ideal_x; ++i) {
        for (int j = 0; j < ideal_y; ++j) {
          *(transfer_block + i + j * ideal_x) = *(grid + (i + global_x) + (j + global_y) * ctrl.x_grid);
        }
      }

      // Processor 0 initializes itself manually
      if (p == 0) {
        for (int i = 0; i < ideal_x; ++i) {
          for (int j = 0; j < ideal_y; ++j) {
            *(pad + i + j * ideal_x) = *(transfer_block + i + j * ideal_x);
          }
        }
      } else {
        // Otherwise send the transfer block to the other processors
        MPI_Send(transfer_block, ideal_x*ideal_y, MPI_DOUBLE_COMPLEX, p, 0, MPI_COMM_WORLD);
      }
      free(transfer_block);
    }
  } else {
    // Calculate the current processors rank and size of its sub-grid based on that
    int ideal_x = ctrl.x_grid / ctrl.nprocs;
    int ideal_y = ctrl.y_grid / ctrl.nprocs;

    int diff_x = (ctrl.nprocs * ideal_x) - ctrl.x_grid;
    int diff_y = (ctrl.nprocs * ideal_y) - ctrl.y_grid;

    if (rank < diff_x) {
      ideal_x++; 
    } 

    if (rank % ctrl.nprocs < diff_y) {
      ideal_y++; 
    }
    
    // Recieve the transfer block and store in its pad
    MPI_Status rec_ic_st;
    MPI_Recv(pad, ideal_x*ideal_y, MPI_DOUBLE_COMPLEX, 0, 0, MPI_COMM_WORLD, &rec_ic_st);
  }
}

/*
 * Function: simulate
 * ----------------------------
 *   Actually generate the julia set image
 *
 *   pad: the sub-grid each processor is working on
 *   img_seg: the sub-section of the image the processor is working on
 *   rank: the index of the current processor
 *
 *   returns: void
 */
void simulate(complex double * pad, uint8_t * img_seg, int rank, double c) {
  // Calculate the size of the current processors sub-grid
  int ideal_x = ctrl.x_grid / ctrl.nprocs;
  int ideal_y = ctrl.y_grid / ctrl.nprocs;

  int diff_x = (ctrl.nprocs * ideal_x) - ctrl.x_grid;
  int diff_y = (ctrl.nprocs * ideal_y) - ctrl.y_grid;

  if (rank < diff_x) {
    ideal_x++; 
  }

  if (rank % ctrl.nprocs < diff_y) {
    ideal_y++; 
  }

  // Iterate through every point in the simulation grid
  for (int i = 0; i < ideal_x; ++i) {
    for (int j = 0; j < ideal_y; ++j) {
      int it = 0;
      double cr = creal(*(pad + i + j * ideal_x));
      double ci = cimag(*(pad + i + j * ideal_x));
      
      // Continue applying the complex map until abs(x+iy) > escape condition or exceed max iterations
      while ((cr * cr + ci * ci) < ctrl.escape && it < ctrl.iters) {
        *(pad + i + j * ideal_x) = map(*(pad + i + j * ideal_x), c);
        cr = creal(*(pad + i + j * ideal_x));
        ci = cimag(*(pad + i + j * ideal_y));
        it++;
      }

      // If we maxed out on iterations we color in black (in the julia set), otherwise color based on iterations
      if (it == ctrl.iters) {
        *(img_seg + i + j * ideal_x) = (uint8_t) 0;
      } else {
        *(img_seg + i + j * ideal_x) = (uint8_t) (it % 256);
      }
    }
  }
}

/*
 * Function: collate
 * ----------------------------
 *   Collect the image sub-sections into the total image
 *
 *   img: The main image we wish to combine the processor subsections into
 *   img_seg: the sub-section of the image the processor calculated
 *   rank: the index of the current processor
 *
 *   returns: void
 */
void collate(uint8_t * img, uint8_t * img_seg, int rank) {
  // All processors but zero will send img segment data to processor 0
  if (rank != 0) {
    int ideal_x = ctrl.x_grid / ctrl.nprocs;
    int ideal_y = ctrl.y_grid / ctrl.nprocs;

    int diff_x = (ctrl.nprocs * ideal_x) - ctrl.x_grid;
    int diff_y = (ctrl.nprocs * ideal_y) - ctrl.y_grid;

    if (rank < diff_x) {
      ideal_x++; 
    } 

    if (rank % ctrl.nprocs < diff_y) {
      ideal_y++; 
    }

    // Processor 0 initializes itself manually
    MPI_Send(img_seg, ideal_x*ideal_y, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
  } else {
    // Processor 0 will iterate through all of the processors and retrieve image segments
    for (int p = 0; p < ctrl.tprocs; ++p) {
      int ideal_x = ctrl.x_grid / ctrl.nprocs;
      int ideal_y = ctrl.y_grid / ctrl.nprocs;

      int diff_x = (ctrl.nprocs * ideal_x) - ctrl.x_grid;
      int diff_y = (ctrl.nprocs * ideal_y) - ctrl.y_grid;

      if (p % ctrl.nprocs < diff_x) {
        ideal_x++; 
      } 

      if (p / ctrl.nprocs < diff_y) {
        ideal_y++; 
      }

      // Temporary grid to recieve data from other processors
      uint8_t * transfer_img = (uint8_t *) malloc(ideal_x * ideal_y * sizeof(uint8_t));

      //int global_x = (p < diff_x ? p * ideal_x : diff_x * (ideal_x + 1)) + (p > diff_x ? (p - diff_y) * ideal_x : 0); 
      //int global_y = ((p % ctrl.nprocs) < diff_y ? p * ideal_y : diff_y * (ideal_y + 1)) + (p > diff_y ? ((p % ctrl.nprocs) - diff_y) * ideal_y : 0);   
      
      int global_x = ((p % ctrl.nprocs) * ideal_x);
      int global_y = ((p / ctrl.nprocs) * ideal_y);
      
      // Processor 0 can copy directly from its own img_segment, no sending needed
      if (p == 0) {
        // Copy img_seg to img
        for (int i = 0; i < ideal_x; ++i) {
          for (int j = 0; j < ideal_y; ++j) {
            *(img + (i + global_x) + (j + global_y) * ctrl.x_grid) = *(img_seg + i + j * ideal_x);
          }
        }
      } else {
        // MPI Receive from all the other processors
        MPI_Status rec_ic_st;
        MPI_Recv(transfer_img, ideal_x*ideal_y, MPI_UNSIGNED_CHAR, p, 0, MPI_COMM_WORLD, &rec_ic_st);
        
        // Copy the transfer_img to the full image grid
        for (int i = 0; i < ideal_x; ++i) {
          for (int j = 0; j < ideal_y; ++j) {
            *(img + (i + global_x) + (j + global_y) * ctrl.x_grid) = *(transfer_img + i + j * ideal_x);
          }
        }
      }
      free(transfer_img);
    }      
  }
}
