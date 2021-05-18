#ifndef grid_h
#define grid_h

#include <complex.h>
#include <stdlib.h>
#include <mpi.h>
#include <stdint.h>

#include "control.h"
#include "map.h"

double complex * init(int rank);
double complex * pad_alloc(int rank);
uint8_t * img_alloc(int rank);
void distribute_ic(complex double * grid, int rank, complex double * pad);
void simulate(complex double * pad, uint8_t * img_seg, int rank);
void collate(uint8_t * img, uint8_t * img_seg, int rank);

#endif
