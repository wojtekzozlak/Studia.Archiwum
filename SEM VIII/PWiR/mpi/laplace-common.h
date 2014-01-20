#ifndef __LAPLACE_COMMON_H__
#define __LAPLACE_COMMON_H__

#include <math.h>


/**
 * Returns the initial value of point (i, j).
 */
static double getInitialValue(int i, int j, int numPointsPerDimension);

/**
 * Returns the relaxation factor.
 */
static double getOmega(int numPointsPerDimension);

/**
 * Returns the tolerance value.
 */
static double getEpsilon(int numPointsPerDimension);

#include "laplace-common-impl.h"

#endif /* __LAPLACE_COMMON_H__ */

