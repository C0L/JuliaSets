/*
 * File:  cmdl.h
 * --------------------
 *   parses command line arguments intro ctrl struct
 */

#ifndef cmdl_h
#define cmdl_h

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>

#include "control.h"

void parse_cmds(int argc, char ** argv); 

#endif
