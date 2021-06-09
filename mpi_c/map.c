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
/*double complex map(double complex z, double c) {
  //return z*z + (0.285 + 0.01*I);
  return z*z + (-.79 + .15*I);
}*/


/*double complex map(double complex z, double c) {
  return c * I * ccos(z);
}*/

/* USED FOR STATIC GENERATION */
//double complex map(double complex z, double c) {
//  return z*z + 0.355534 - .337292*I;
//}


/* USED FOR DYNAMIC GENERATION */
double complex map(double complex z, double c) {
  return z*z + .7885*cexp(I*c);
}

