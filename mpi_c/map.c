#include "map.h"

double complex map(double complex z) {
  return z*z + (0.285 + 0.01*I);
}
