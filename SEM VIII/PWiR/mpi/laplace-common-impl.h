#ifndef __LAPLACE_COMMON_IMPL_001_H__
#define __LAPLACE_COMMON_IMPL_001_H__

#define LAPLACE_I0 4.56
#define LAPLACE_IN 9.85
#define LAPLACE_J0 7.32
#define LAPLACE_JN 6.88
#define LAPLACE_XX 0.0


static inline double getInitialValue(int i, int j, int numPointsPerDimension)
{
    if (i == 0)
    {
        return LAPLACE_I0;
    }
    else if (i == numPointsPerDimension - 1)
    {
        return LAPLACE_IN;
    }
    else if (j == 0)
    {
        return LAPLACE_J0;
    }
    else if (j == numPointsPerDimension - 1)
    {
        return LAPLACE_JN;
    }
    else
    {
        return LAPLACE_XX;
    }
}



static inline double getOmega(int numPointsPerDimension)
{
    double p = cos(M_PI / (double)numPointsPerDimension);
    return 1.6 / (1 + sqrt(1 - p * p));
}



static inline double getEpsilon(int numPointsPerDimension)
{
    return 0.000002 / (2.0 - 1.25 * getOmega(numPointsPerDimension));
}



#endif /* __LAPLACE_COMMON_IMPL_001_H__ */

