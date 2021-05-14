#ifndef cmdl_h
#define cmdl_h

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "control.h"

void parse_cmds(int argc, char ** argv, int * x_grid, int * y_grid, int * nprocs); // TODO add function parse

#endif
