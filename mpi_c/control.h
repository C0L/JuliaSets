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

} control;

extern control ctrl;

#endif
