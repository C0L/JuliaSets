#include "cmdl.h"

static struct option options[] = {
  {"i", required_argument, 0, 'i'},
  {"n", required_argument, 0, 'n'},
  {"s", required_argument, 0, 's'},
  {"r", required_argument, 0, 'r'},
  {"e", required_argument, 0, 'e'}
};

void parse_cmds(int argc, char ** argv) { 
  /* Parse command line options */
  int c;
  int isqrt;
  float fsqrt;
  while ((c = getopt_long(argc, argv, "s:n:i:r:e:", options, NULL)) != -1) {
    switch (c) {
      case 's':
        ctrl.y_grid = atoi(optarg);
        ctrl.x_grid = atoi(optarg);
        break;
      case 'n':
        fsqrt = sqrt((double) atoi(optarg));
        isqrt = fsqrt;
        if (isqrt != fsqrt) {
          printf("nprocs is not square\n");
          exit(-1);
        }
        ctrl.nprocs = isqrt;
        ctrl.tprocs = atoi(optarg);
        break;
      case 'i':
        ctrl.iters = atoi(optarg);
        break; 
      case 'r':
        ctrl.region = atof(optarg);
        break; 
      case 'e':
        ctrl.escape = atof(optarg);
        break;       
      default:
        printf("Usage: juliaset [-i <iterations>] [-n <number procs>] [-s <x/y dim>]\n");
        exit(-1);
    }
  }
}
