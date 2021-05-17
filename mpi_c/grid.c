#include "grid.h"

control ctrl;

// Allows us to configure what is distributed to processors
void init(double complex * grid, int rank) {
  if (rank == 0) {
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

      int ideal_x = ctrl.x_grid / p;
      int ideal_y = ctrl.y_grid / p;

      int diff_x = (ctrl.nprocs * ideal_x) - ctrl.x_grid;
      int diff_y = (ctrl.nprocs * ideal_y) - ctrl.y_grid;

      if (rank < diff_x) {
        ideal_x++; 
      } 

      if (rank % ctrl.nprocs < diff_y) {
        ideal_y++; 
      }
 
      complex double * transfer_block = (complex double *) malloc(ideal_x * ideal_y * sizeof(complex double));

      int global_x = (rank < diff_x ? rank * ideal_x : rank * (ideal_x + 1)) + (rank > diff_x ? rank * ideal_x : 0); 
      int global_y = ((rank % ctrl.nprocs) < diff_y ? rank * ideal_y : (rank % ctrl.nprocs) * (ideal_y + 1)) + (rank > diff_y ? (rank % ctrl.nprocs) * ideal_y : 0);   


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
        MPI_Request send_ic_rq;
        MPI_Status send_ic_st;

        MPI_Isend(transfer_block, ideal_x*ideal_y, MPI_DOUBLE_COMPLEX, p, 0, MPI_COMM_WORLD, &send_ic_rq);

        MPI_Wait(&send_ic_rq, &send_ic_st);
      }
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
    MPI_Request rec_ic_rq;
    MPI_Status rec_ic_st;

    MPI_Irecv(pad, ideal_x*ideal_y, MPI_DOUBLE_COMPLEX, 0, 0, MPI_COMM_WORLD, &rec_ic_rq);

    MPI_Wait(&rec_ic_rq, &rec_ic_st);
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

  for (int i = 0; i <ctrl.x_grid; ++i) {
    for (int j = 0; j < ctrl.y_grid; ++j) {
      int it = 0;
      double cr = creal(*(pad + i + j * ideal_x));
      double ci = cimag(*(pad + i + j * ideal_x));

      while ((cr * cr + ci * ci) < ctrl.escape && it < ctrl.iters) {
        *(pad + i + j * ctrl.x_grid) = map(*(pad + i + j * ctrl.x_grid));
        cr = creal(*(pad + i + j * ctrl.x_grid));
        ci = cimag(*(pad + i + j * ctrl.y_grid));
        it++;
      }

      if (it == ctrl.iters) {
        *(img_seg + i + j * ctrl.x_grid) = (uint8_t) 0;
      } else {
        *(img_seg + i + j * ctrl.x_grid) = (uint8_t) (it % 256);
      }
    }
  }
}

void collate() {

}
