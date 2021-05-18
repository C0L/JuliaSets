#include "grid.h"
#include <stdio.h>

// Allows us to configure what is distributed to processors
double complex * init(int rank) {
  if (rank == 0) {
    printf("Construct IC\n");
    double complex * grid = (double complex *) malloc(ctrl.y_grid * ctrl.x_grid * sizeof(double complex));

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

double complex * pad_alloc(int rank) {
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

  double complex * pad  = (complex double *) malloc(ideal_x * ideal_y * sizeof(double complex));

  return pad;
}

uint8_t * img_alloc(int rank) {
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

  uint8_t * pad  = (uint8_t*) malloc(ideal_x * ideal_y * sizeof(uint8_t));

  return pad;
}

void distribute_ic(complex double * grid, int rank, complex double * pad) {
  if (rank == 0) {
    // MPI Send IC and copy direct for rank 0
    for (int p = 0; p < ctrl.tprocs; ++p) {

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
 
      complex double * transfer_block = (complex double *) malloc(ideal_x * ideal_y * sizeof(complex double));

      int global_x = (p < diff_x ? p * ideal_x : p * (ideal_x + 1)) + (p > diff_x ? p * ideal_x : 0); 
      int global_y = ((p % ctrl.nprocs) < diff_y ? p * ideal_y : (p % ctrl.nprocs) * (ideal_y + 1)) + (p > diff_y ? (p % ctrl.nprocs) * ideal_y : 0);   


      for (int i = 0; i < ideal_x; ++i) {
        for (int j = 0; j < ideal_y; ++j) {
          *(transfer_block + i + j*ideal_x) = *(grid + (i + global_x) + (j+global_y) * ctrl.x_grid);
        }
      }

      // Processor 0 initializes itself manually
      if (p == 0) {
        for (int i = 0; i < ideal_x; ++i) {
          for (int j = 0; j < ideal_y; ++j) {
            *(pad + i + j*ideal_x) = *(transfer_block + i + j*ideal_x);
          }
        }
      } else {
        //MPI_Request send_ic_rq;
        //MPI_Status send_ic_st;

        MPI_Send(transfer_block, ideal_x*ideal_y, MPI_DOUBLE_COMPLEX, p, 0, MPI_COMM_WORLD);
        //MPI_Isend(transfer_block, ideal_x*ideal_y, MPI_DOUBLE_COMPLEX, p, 0, MPI_COMM_WORLD, &send_ic_rq);

        //MPI_Wait(&send_ic_rq, &send_ic_st);
      }
      free(transfer_block);
    }
  } else {
    int ideal_x = ctrl.x_grid / rank;
    int ideal_y = ctrl.y_grid / rank;

    int diff_x = (ctrl.nprocs * ideal_x) - ctrl.x_grid;
    int diff_y = (ctrl.nprocs * ideal_y) - ctrl.y_grid;

    if (rank < diff_x) {
      ideal_x++; 
    } 

    if (rank % ctrl.nprocs < diff_y) {
      ideal_y++; 
    }
 
     // MPI Receive
   // MPI_Request rec_ic_rq;
    MPI_Status rec_ic_st;

    //MPI_Irecv(pad, ideal_x*ideal_y, MPI_DOUBLE_COMPLEX, 0, 0, MPI_COMM_WORLD, &rec_ic_rq);

    MPI_Recv(pad, ideal_x*ideal_y, MPI_DOUBLE_COMPLEX, 0, 0, MPI_COMM_WORLD, &rec_ic_st);
    //MPI_Wait(&rec_ic_rq, &rec_ic_st);
  }
}

void simulate(complex double * pad, uint8_t * img_seg, int rank) {
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

  for (int i = 0; i < ideal_x; ++i) {
    for (int j = 0; j < ideal_y; ++j) {
      int it = 0;
      double cr = creal(*(pad + i + j * ideal_x));
      double ci = cimag(*(pad + i + j * ideal_x));

      while ((cr * cr + ci * ci) < ctrl.escape && it < ctrl.iters) {
        *(pad + i + j * ideal_x) = map(*(pad + i + j * ideal_x));
        cr = creal(*(pad + i + j * ideal_x));
        ci = cimag(*(pad + i + j * ideal_y));
        it++;
      }

      if (it == ctrl.iters) {
        *(img_seg + i + j * ideal_x) = (uint8_t) 0;
      } else {
        *(img_seg + i + j * ideal_x) = (uint8_t) (it % 256);
      }
    }
  }
}

void collate(uint8_t * img, uint8_t * img_seg, int rank) {
  // Just recieve on proc 0 and copy its own img segment
  if (rank != 0) {
    int ideal_x = ctrl.x_grid / rank;
    int ideal_y = ctrl.y_grid / rank;

    int diff_x = (ctrl.nprocs * ideal_x) - ctrl.x_grid;
    int diff_y = (ctrl.nprocs * ideal_y) - ctrl.y_grid;

    if (rank < diff_x) {
      ideal_x++; 
    } 

    if (rank % ctrl.nprocs < diff_y) {
      ideal_y++; 
    }

    // Processor 0 initializes itself manually
//    MPI_Request send_ic_rq;
//    MPI_Status send_ic_st;

    MPI_Send(img_seg, ideal_x*ideal_y, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);

//    MPI_Wait(&send_ic_rq, &send_ic_st);
  } else {
    for (int p = 0; p < ctrl.tprocs; ++p) {
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
 
      uint8_t * transfer_img = (uint8_t *) malloc(ideal_x * ideal_y * sizeof(uint8_t));

      int global_x = (p < diff_x ? p * ideal_x : diff_x * (ideal_x + 1)) + (p > diff_x ? (p - diff_y) * ideal_x : 0); 
      int global_y = ((p % ctrl.nprocs) < diff_y ? p * ideal_y : diff_y * (ideal_y + 1)) + (p > diff_y ? ((p % ctrl.nprocs) - diff_y) * ideal_y : 0);   

      if (p == 0) {
        for (int i = 0; i < ideal_x; ++i) {
          for (int j = 0; j < ideal_y; ++j) {
            *(img + (i + global_x) + (j + global_y) * ctrl.x_grid) = *(img_seg + i + j*ideal_x);
          }
        }
      } else {
        // MPI Receive
//        MPI_Request rec_ic_rq;
        MPI_Status rec_ic_st;

        MPI_Recv(transfer_img, ideal_x*ideal_y, MPI_UNSIGNED_CHAR, p, 0, MPI_COMM_WORLD, &rec_ic_st);

        //MPI_Wait(&rec_ic_rq, &rec_ic_st);

        for (int i = 0; i < ideal_x; ++i) {
          for (int j = 0; j < ideal_y; ++j) {
            *(img + (i + global_x) + (j + global_y) * ctrl.x_grid) = *(transfer_img + i + j * ideal_x);
            //*(img) = *(transfer_img + i + j*ideal_x);
          }
        }
      }
      free(transfer_img);
    }      
  }
}
