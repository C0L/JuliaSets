#include "cmdl.h"

static struct option options[] = {
  {"i", required_argument, 0, 'i'},
  {"n", required_argument, 0, 'n'},
  {"x", required_argument, 0, 'x'},
  {"y", required_argument, 0, 'y'}
};

extern control ctrl;

void parse_cmds(int argc, char ** argv) { 
  /* Parse command line options */
  int c;
  while ((c = getopt_long(argc, argv, "n:x:y", options, NULL)) != -1) {
    switch (c) {
      case 'x':
        ctrl.x_grid = atoi(optarg);
        break;
      case 'y':
        ctrl.y_grid = atoi(optarg);
        break;
      case 'n':
        ctrl.nprocs = atoi(optarg);
        break;
      case 'i':
        ctrl.iters = atoi(optarg);
        break; 
      default:
        abort();
    }
  }
}
