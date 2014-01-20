#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <mpi.h>
#include "laplace-common.h"
#include <unistd.h>

#define OPTION_VERBOSE "--verbose"
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

#define MPI_MESSAGE_RAW_DATA_TAG 50
#define MPI_MESSAGE_DATA_TAG 51
#define MPI_MESSAGE_DIFF_TAG 52
#define MPI_MESSAGE_FINAL_DATA_TAG 53

// 1GiB (in doubles)
#define MAX_MSG_LEN 130000


static void printUsage(char const * progName) {
  fprintf(stderr, "Usage:\n"
      "    %s [--verbose] <N>\n"
      "Where:\n"
      "   <N>         The number of points in each dimension (at least 2).\n"
      "   " OPTION_VERBOSE "   Prints the input and output systems.\n",
      progName); 
}


static void terminate(int code) {
  MPI_Finalize();
  exit(code);
}


static double* allocBuffer(int size) {
  double* b = (double*)malloc(sizeof(double) * size);
  if (b == NULL) {
    fprintf(stderr, "error: malloc; allocBuffer\n");
    terminate(1);
  }
  return b;
};


static void freePoints(double** points, int rows) {
  if (points != NULL) {
    free(*points);
    free(points);
  }
}


static double** allocatePoints(int rows, int cols) {
  double** points;
  double* wholeData;
  int i;

  wholeData = (double*)malloc(sizeof(double) * cols * rows);

  points = (double**)malloc(sizeof(double*) * rows);
  if (points == NULL) {
    goto FAILURE;
  }
  for (i = 0; i < rows; ++i) {
    points[i] = wholeData + i * cols;
  }
  return points;

FAILURE:
  freePoints(points, rows);
  return NULL;
}


static void initializePoints(double** points, int numPointsPerDimension,
    int lowerBound, int rows) {
  int i, j;
  for (i = 1; i <= rows; ++i) {
    for (j = 0; j < numPointsPerDimension; ++j) {
      points[i][j] = getInitialValue(lowerBound + i - 1, j,
          numPointsPerDimension);
    }
  }
}


static void printPoints(double** points, int start, int end, int cols) {
  int i, j;
  for (i = start; i <= end; ++i) {
    printf("%.10f", points[i][0]);
    for (j = 1; j < cols; ++j) {
      printf(" %.10f", points[i][j]);
    }
    printf("\n");
  }
}


void sendRawData(double* data, int size, int to) {
  int offset = 0;
  int partSize;
  int retval;

  do {
    partSize = MIN(MAX_MSG_LEN, size - offset);

    retval = MPI_Ssend(data + offset, partSize, MPI_DOUBLE, to,
        MPI_MESSAGE_RAW_DATA_TAG, MPI_COMM_WORLD);
    if (retval != MPI_SUCCESS) {
      fprintf(stderr, "error: MPI_Ssend; raw data %d\n", retval);
      terminate(1);
    }

    offset += partSize;
  } while(offset != size);
}


void recvRawData(double* data, int size) {
  int offset = 0;
  int partSize;
  int retval;
  MPI_Status status;

  do {
    partSize = MIN(MAX_MSG_LEN, size - offset);

    retval = MPI_Recv(data + offset, partSize, MPI_DOUBLE, 0,
        MPI_MESSAGE_RAW_DATA_TAG, MPI_COMM_WORLD, &status);
    if (retval != MPI_SUCCESS) {
      fprintf(stderr, "error: MPI_Recv; raw data\n");
      terminate(1);
    }

    offset += partSize;
  } while(offset != size);
}


void sendComputationData(double** points, int rows, int cols,
    int myProcessNo, int numProcesses) {
  static MPI_Request upperRequest;
  static double* upperBuffer = NULL;
  static MPI_Request lowerRequest;
  static double* lowerBuffer = NULL;

  int status;
  if (myProcessNo != 0) {
    if (lowerBuffer == NULL)
      lowerBuffer = allocBuffer(cols);
    else 
      MPI_Wait(&lowerRequest, MPI_STATUS_IGNORE);
    memcpy(lowerBuffer, points[1], sizeof(double) * cols);

    status = MPI_Isend(lowerBuffer, cols, MPI_DOUBLE, myProcessNo - 1,
        MPI_MESSAGE_DATA_TAG, MPI_COMM_WORLD, &lowerRequest);
    if (status != MPI_SUCCESS) {
      fprintf(stderr, "error: MPI_Isend; lower data");
      terminate(1);
    }
  }
  if (myProcessNo != numProcesses - 1) {
    if (upperBuffer == NULL)
      upperBuffer = allocBuffer(cols);
    else
      MPI_Wait(&upperRequest, MPI_STATUS_IGNORE);
    memcpy(upperBuffer, points[rows], sizeof(double) * cols);


    status = MPI_Isend(upperBuffer, cols, MPI_DOUBLE, myProcessNo + 1,
        MPI_MESSAGE_DATA_TAG, MPI_COMM_WORLD, &upperRequest);
    if (status != MPI_SUCCESS) {
      fprintf(stderr, "error: MPI_Isend; upper data");
      terminate(1);
    }
  }
}


void getComputationData(double** points, int rows, int cols,
    int myProcessNo, int numProcesses) {
  int retval;
  MPI_Status status;
  if (myProcessNo != 0) {
    retval = MPI_Recv(points[0], cols, MPI_DOUBLE, myProcessNo - 1,
        MPI_MESSAGE_DATA_TAG, MPI_COMM_WORLD, &status);
    if (retval != MPI_SUCCESS) {
      fprintf(stderr, "error: MPI_recv; lower data");
      terminate(1); 
    }
  }
  if (myProcessNo != numProcesses - 1) {
    retval = MPI_Recv(points[rows + 1], cols, MPI_DOUBLE, myProcessNo + 1,
        MPI_MESSAGE_DATA_TAG, MPI_COMM_WORLD, &status);
    if (retval != MPI_SUCCESS) {
      fprintf(stderr, "error: MPI_recv; upper data");
      terminate(1); 
    }  
  }
}


void sendComputedData(double** points, int rows, int cols, int n) {
  int i, retval;
  for (i = 1; i <= rows; i++) {
    retval = MPI_Ssend(points[i], cols, MPI_DOUBLE, 0,
        MPI_MESSAGE_FINAL_DATA_TAG, MPI_COMM_WORLD);
    if (retval != MPI_SUCCESS) {
      fprintf(stderr, "error: MPI_send; verbose %d\n", retval);
      terminate(1);
    }
  }
};


void receiveAndPrintComputedData(double* buffer, int cols, int from) {
  MPI_Status status;
  int retval;
  retval = MPI_Recv(buffer, cols, MPI_DOUBLE, from,
      MPI_MESSAGE_FINAL_DATA_TAG, MPI_COMM_WORLD, &status);
  if (retval != MPI_SUCCESS) {
    fprintf(stderr, "error: MPI_Recv; verbose\n");
    terminate(1);
  }
  printPoints(&buffer, 0, 0, cols);
};


int main(int argc, char* argv[]) {
  int numProcesses;
  int myProcessNo;


  if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
    fprintf(stderr, "error: MPI_Init\n");
    exit(1);
  }

  if (MPI_Comm_size(MPI_COMM_WORLD, &numProcesses) != MPI_SUCCESS) {
    fprintf(stderr, "error: MPI_Comm_size\n");
    terminate(1);
  }

  if (MPI_Comm_rank(MPI_COMM_WORLD, &myProcessNo) != MPI_SUCCESS) {
    fprintf(stderr, "error: MPI_Comm_rank\n");
    terminate(1);
  }


  int numPointsPerDimension;
  int verbose = 0;
  int rowsPerProcess;
  int rows;
  int lowerBound;
  int upperBound;
  double omega;
  double epsilon;
  double** points;
  struct timeval startTime;
  struct timeval endTime;
  struct timeval compStartTime;
  struct timeval compEndTime;
  struct timeval wholeStartTime;
  struct timeval wholeEndTime;
  long long compTime = 0;
  double duration;
  int numIterations;
  double maxDiff;
  double reducedDiff;

  if (argc < 2) {
    fprintf(stderr, "ERROR: Too few arguments!\n");
    printUsage(argv[0]);
    terminate(1);
  } else if (argc > 3) {
    fprintf(stderr, "ERROR: Too many arguments!\n");
    printUsage(argv[0]);
    terminate(1);
  } else {
    int argIdx = 1;
    if (argc == 3) {
      if (strncmp(argv[argIdx], OPTION_VERBOSE, strlen(OPTION_VERBOSE)) != 0) {
        fprintf(stderr, "ERROR: Unexpected option '%s'!\n", argv[argIdx]);
        printUsage(argv[0]);
        exit(1);
      }
      verbose = 1;
      ++argIdx;
    }
    numPointsPerDimension = atoi(argv[argIdx]);
    if (numPointsPerDimension < 2) {
      fprintf(stderr, "ERROR: The number of points, '%s', should be "
          "a numeric value greater than or equal to 2!\n", argv[argIdx]);
      printUsage(argv[0]);
      terminate(1);
    }
  }

  if(gettimeofday(&wholeStartTime, NULL)) {
    fprintf(stderr, "ERROR: Gettimeofday failed!\n");
    terminate(1);
  }


  rowsPerProcess = (numPointsPerDimension / numProcesses) +
      (numPointsPerDimension % numProcesses == 0 ? 0 : 1);
  numProcesses = numPointsPerDimension / rowsPerProcess +
      (numPointsPerDimension % rowsPerProcess == 0 ? 0 : 1);

  lowerBound = rowsPerProcess * myProcessNo;
  upperBound = MIN(rowsPerProcess * (myProcessNo + 1) - 1,
      numPointsPerDimension - 1);
  rows = MAX(upperBound - lowerBound + 1, 0);

  omega = getOmega(numPointsPerDimension);
  epsilon = getEpsilon(numPointsPerDimension);
  points = allocatePoints(rows + 2, numPointsPerDimension);

  if (rows == 0) {
    do {
      maxDiff = 0.0;
      MPI_Allreduce(&maxDiff, &reducedDiff, 1, MPI_DOUBLE, MPI_MAX,
          MPI_COMM_WORLD); 
    } while (reducedDiff > epsilon); 
    terminate(0);
  }


  if (points == NULL) {
    freePoints(points, numPointsPerDimension);
    fprintf(stderr, "ERROR: Malloc failed! %d \n", myProcessNo);
    terminate(1);
  }

  if (myProcessNo == 0) {
    int i;
    for (i = numProcesses - 1; i >= 0; --i) {
      lowerBound = rowsPerProcess * i;
      upperBound = MIN(rowsPerProcess * (i + 1) - 1,
          numPointsPerDimension - 1);
      rows = MAX(upperBound - lowerBound + 1, 0);
      initializePoints(points, numPointsPerDimension, lowerBound, rows);
      
      if (i == 0) // preserve rows and data
        break;

      sendRawData(points[1], rows * numPointsPerDimension, i);
    }
  } else {
    recvRawData(points[1], rows * numPointsPerDimension);
  }

 
  if(gettimeofday(&startTime, NULL)) {
    freePoints(points, numPointsPerDimension);
    fprintf(stderr, "ERROR: Gettimeofday failed!\n");
    terminate(1);
  }



  /* Start of computations. */
  numIterations = 0;
  do {
    int i, j, color;
    maxDiff = 0.0;
    for (color = 0; color < 2; ++color) {
      sendComputationData(points, rows, numPointsPerDimension, myProcessNo,
          numProcesses);
      getComputationData(points, rows, numPointsPerDimension, myProcessNo,
          numProcesses);



      if(gettimeofday(&compStartTime, NULL)) {
        freePoints(points, numPointsPerDimension);
        fprintf(stderr, "ERROR: Gettimeofday failed!\n");
        terminate(1);
      }

      int startRow = lowerBound == 0 ? 2 : 1;
      int endRow = upperBound == numPointsPerDimension - 1 ? rows - 1 : rows;

      for (i = startRow; i <= endRow; ++i) {
        int actualRow = i + lowerBound - 1;
        for (j = 1 + (actualRow % 2 == color ? 1 : 0);
            j < numPointsPerDimension - 1; j += 2) {
          double tmp, diff;
          tmp = (points[i - 1][j] + points[i + 1][j] + points[i][j - 1] + points[i][j + 1]) / 4.0;
          diff = points[i][j];
          points[i][j] = (1.0 - omega) * points[i][j] + omega * tmp;
          diff = fabs(diff - points[i][j]);
          if (diff > maxDiff) {
            maxDiff = diff;
          }
        }
      }

      if(gettimeofday(&compEndTime, NULL)) {
        freePoints(points, numPointsPerDimension);
        fprintf(stderr, "ERROR: Gettimeofday failed!\n");
        terminate(1);
      }
      compTime += (compEndTime.tv_sec * 1000000 + compEndTime.tv_usec) -
                  (compStartTime.tv_sec * 1000000 + compStartTime.tv_usec); 
    }
    ++numIterations;

    MPI_Allreduce(&maxDiff, &reducedDiff, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);    maxDiff = reducedDiff;
  }
  while (maxDiff > epsilon);


  /* End of computations. */

  if(gettimeofday(&endTime, NULL)) {
    freePoints(points, numPointsPerDimension);
    fprintf(stderr, "ERROR: Gettimeofday failed!\n");
    terminate(1);
  }

  duration =
      ((double)endTime.tv_sec + ((double)endTime.tv_usec / 1000000.0)) - 
      ((double)startTime.tv_sec + ((double)startTime.tv_usec / 1000000.0));

  if (myProcessNo == 0) {
#ifndef WITH_BREAKDOWN
    fprintf(stderr,
        "Statistics: duration(s)=%.10f #iters=%d diff=%.10f epsilon=%.10f\n",
        duration, numIterations, maxDiff, epsilon);
#else
    double compDuration = (double)compTime / 1000000.0;
    fprintf(stderr,
        "Statistics: duration(s)=%.10f #iters=%d diff=%.10f epsilon=%.10f computation=%.10f\n",
        duration, numIterations, maxDiff, epsilon, compDuration);
#endif
  }

  if (verbose) {
    if (myProcessNo != 0) {
      sendComputedData(points, rows, numPointsPerDimension, myProcessNo);
    } else {
      printPoints(points, 1, rows, numPointsPerDimension);

      int i, j;
      for (i = rows, j = 1; i < numPointsPerDimension; i++) {
        receiveAndPrintComputedData(points[0], numPointsPerDimension, j);
        if ((i + 1) % rows == 0)
          j++;
      }
    }    
  }

  freePoints(points, rows + 2);

  if(gettimeofday(&wholeEndTime, NULL)) {
    freePoints(points, numPointsPerDimension);
    fprintf(stderr, "ERROR: Gettimeofday failed!\n");
    terminate(1);
  }

#ifdef WITH_BREAKDOWN
  if (myProcessNo == 0) {
    duration =
        ((double)wholeEndTime.tv_sec +
            ((double)wholeEndTime.tv_usec / 1000000.0)) - 
        ((double)wholeStartTime.tv_sec +
            ((double)wholeStartTime.tv_usec / 1000000.0));
    fprintf(stderr, "Whole time: %.10f\n", duration);
  }
#endif

  MPI_Finalize();
  return 0;
}

