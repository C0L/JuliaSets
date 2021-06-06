/*
 * File:  map.h
 * --------------------
 *   complex map used in generation of julia set
 */

#include "map.h"

/*
 * Function: map
 * ----------------------------
 *   Complex mapping iteratively mapped over complex plane
 *
 *   z: current complex value
 *
 *   returns: mapped complex value
 */
double complex map(double complex z) {
  //return z*z + (0.285 + 0.01*I);
  return z*z + (-.79 + .15*I);
}
