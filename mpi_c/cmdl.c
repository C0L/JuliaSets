#include "cmdl.h"

static struct option options[] {
  {"n"}

}

void parse_cmds(int argc, char ** argv, int * x_grid, int * y_grid, int * nprocs) { // TODO add function parse
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
