/*
 * File:  cmdl.c
 * --------------------
 *   parses command line arguments intro ctrl struct
 */

#include "cmdl.h"

// Valid cmd line arguments
static struct option options[] = {
  {"i", required_argument, 0, 'i'},
  {"n", required_argument, 0, 'n'},
  {"s", required_argument, 0, 's'},
  {"r", required_argument, 0, 'r'},
  {"e", required_argument, 0, 'e'}
};

/*
 * Function: parse_cmds
 * ----------------------------
 *   Parses command from the user into control ctrl
 *
 *   argc: number of arguments
 *   argv: arguments
 *
 *   returns: void
 */
void parse_cmds(int argc, char ** argv) { 
  /* Parse command line options */
  int c;
  int isqrt;
  float fsqrt;
  while ((c = getopt_long(argc, argv, "s:n:i:r:e:", options, NULL)) != -1) {
    switch (c) {
      case 's':
        // Get the simulation grid size
        ctrl.y_grid = atoi(optarg);
        ctrl.x_grid = atoi(optarg);
        break;
      case 'n':
        // Get the number of processors and make sure it is perfect square
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
        // Get the max number of iterations
        ctrl.iters = atoi(optarg);
        break; 
      case 'r':
        // Get the region of complex plane to simulate
        ctrl.region = atof(optarg);
        break; 
      case 'e':
        // Get the escape condition
        ctrl.escape = atof(optarg);
        break;       
      default:
        // TODO this needs to be updated
        printf("Usage: juliaset [-i <iterations>] [-n <number procs>] [-s <x/y dim>]\n");
        exit(-1);
    }
  }
}
