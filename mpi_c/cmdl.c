#include "cmdl.h"

static struct option options[] {
  {"n", required_argument, 0, 'n'},
  {"x", required_argument, 0, 'x'},
  {"y", required_argument, 0, 'y'},
}

void parse_cmds(int argc, char ** argv, int * x_grid, int * y_grid, int * nprocs) { 
  /* Parse command line options */
  int c;
  while ((c = getopt_long(argc, argv, "x:y")) != -1) {
    switch (c) {
      case 'x':
        *x_grid = atoi(optarg);
        break;
      case 'y':
        *y_grid = atoi(optarg);
        break;
      case 'n':
        
      default:
        abort();
    }
  }
}
