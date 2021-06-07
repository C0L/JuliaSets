/*
 * File:  control.h
 * --------------------
 *   Global atrributes for the entireity of the program (grid size, proc number, escape condition..)
 */

#ifndef control_h
#define control_h

typedef struct control_t {
  int x_grid;
  int y_grid;
  int nprocs;
  int tprocs;
  int iters;
  double region;
  double escape;
  int dynamic;
  double min;
  double max;
  double delta;
  char * ofile;
} control;

extern control ctrl;

#endif
