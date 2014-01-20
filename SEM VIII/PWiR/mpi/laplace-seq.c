#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "laplace-common.h"

#define OPTION_VERBOSE "--verbose"



static void printUsage(char const * progName)
{
    fprintf(stderr, "Usage:\n"
        "    %s [--verbose] <N>\n"
        "Where:\n"
        "   <N>         The number of points in each dimension (at least 2).\n"
        "   " OPTION_VERBOSE "   Prints the input and output systems.\n",
        progName); 
}



static void freePointRow(double * currentPointRow)
{
    if (currentPointRow != NULL)
    {
        free(currentPointRow);
    }
}



static double * allocatecurrentPointRow(int numPointsPerDimension)
{
    return (double *)malloc(sizeof(double) * numPointsPerDimension);
}



static void freePoints(double * * points, int numPointsPerDimension)
{
    if (points != NULL)
    {
        int i;
        for (i = 0; i < numPointsPerDimension; ++i)
        {
            freePointRow(points[i]);
        }
        free(points);
    }
}



static double * * allocatePoints(int numPointsPerDimension)
{
    double * * points;
    int i;

    points = (double * *)malloc(sizeof(double *) * numPointsPerDimension);
    if (points == NULL)
    {
        goto FAILURE;
    }
    for (i = 0; i < numPointsPerDimension; ++i)
    {
        points[i] = NULL;
    }
    for (i = 0; i < numPointsPerDimension; ++i)
    {
        points[i] = allocatecurrentPointRow(numPointsPerDimension);
        if (points[i] == NULL)
        {
            goto FAILURE;
        }
    }
    return points;
FAILURE:
    freePoints(points, numPointsPerDimension);
    return NULL;
}



static void initializePoints(double * * points, int numPointsPerDimension)
{
    int i, j;
    for (i = 0; i < numPointsPerDimension; ++i)
    {
        for (j = 0; j < numPointsPerDimension; ++j)
        {
            points[i][j] = getInitialValue(i, j, numPointsPerDimension);
        }
    }
}



static void printPoints(double * * points, int numPointsPerDimension)
{
    int i, j;
    for (i = 0; i < numPointsPerDimension; ++i)
    {
        printf("%.10f", points[i][0]);
        for (j = 1; j < numPointsPerDimension; ++j)
        {
            printf(" %.10f", points[i][j]);
        }
        printf("\n");
    }
}



int main(int argc, char * argv[])
{
    int numPointsPerDimension;
    int verbose = 0;
    double omega;
    double epsilon;
    double * * points;
    struct timeval startTime;
    struct timeval endTime;
    double duration;
    int numIterations;
    double maxDiff;

    if (argc < 2)
    {
        fprintf(stderr, "ERROR: Too few arguments!\n");
        printUsage(argv[0]);
        exit(1);
    }
    else if (argc > 3)
    {
        fprintf(stderr, "ERROR: Too many arguments!\n");
        printUsage(argv[0]);
        exit(1);
    }
    else
    {
        int argIdx = 1;
        if (argc == 3)
        {
            if (strncmp(argv[argIdx], OPTION_VERBOSE, strlen(OPTION_VERBOSE)) != 0)
            {
                fprintf(stderr, "ERROR: Unexpected option '%s'!\n", argv[argIdx]);
                printUsage(argv[0]);
                exit(1);
            }
            verbose = 1;
            ++argIdx;
        }
        numPointsPerDimension = atoi(argv[argIdx]);
        if (numPointsPerDimension < 2)
        {
            fprintf(stderr, "ERROR: The number of points, '%s', should be "
                "a numeric value greater than or equal to 2!\n", argv[argIdx]);
            printUsage(argv[0]);
            exit(1);
        }
    }

    omega = getOmega(numPointsPerDimension);
    epsilon = getEpsilon(numPointsPerDimension);
    points = allocatePoints(numPointsPerDimension);
    if (points == NULL)
    {
        freePoints(points, numPointsPerDimension);
        fprintf(stderr, "ERROR: Malloc failed!\n");
        exit(1);
    }
    initializePoints(points, numPointsPerDimension);
    
    if(gettimeofday(&startTime, NULL))
    {
        freePoints(points, numPointsPerDimension);
        fprintf(stderr, "ERROR: Gettimeofday failed!\n");
        exit(1);
    }

    /* Start of computations. */

    numIterations = 0;
    do
    {
        int i, j, color;
        maxDiff = 0.0;
        for (color = 0; color < 2; ++color)
        {
            for (i = 1; i < numPointsPerDimension - 1; ++i)
            {
                for (j = 1 + (i % 2 == color ? 1 : 0); j < numPointsPerDimension - 1; j += 2)
                {
                    double tmp, diff;
                    tmp = (points[i - 1][j] + points[i + 1][j] + points[i][j - 1] + points[i][j + 1]) / 4.0;
                    diff = points[i][j];
                    points[i][j] = (1.0 - omega) * points[i][j] + omega * tmp;
                    diff = fabs(diff - points[i][j]);
                    if (diff > maxDiff)
                    {
                        maxDiff = diff;
                    }
                }
            }
        }
        ++numIterations;
    }
    while (maxDiff > epsilon);

    /* End of computations. */

    if(gettimeofday(&endTime, NULL))
    {
        freePoints(points, numPointsPerDimension);
        fprintf(stderr, "ERROR: Gettimeofday failed!\n");
        exit(1);
    }

    duration =
        ((double)endTime.tv_sec + ((double)endTime.tv_usec / 1000000.0)) - 
        ((double)startTime.tv_sec + ((double)startTime.tv_usec / 1000000.0));

    fprintf(stderr,
        "Statistics: duration(s)=%.10f #iters=%d diff=%.10f epsilon=%.10f\n",
        duration, numIterations, maxDiff, epsilon);

    if (verbose)
    {
        printPoints(points, numPointsPerDimension);
    }

    freePoints(points, numPointsPerDimension);
    return 0;
}

