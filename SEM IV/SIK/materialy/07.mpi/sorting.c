#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

#define TAG_D 50
#define TAG_G 51
#define TAG_S 52

static int distribute_values(int rank, int procs) {
  int        dst, value, my_value;
  MPI_Status status;
  
  if (rank == 0) {
    srand(time(NULL));
    my_value = rand();
    printf("0: %d\n", my_value);
    for (dst = 1; dst < procs; dst++) {
      value = rand();
      printf("%d: %d\n", dst, value);
      MPI_Send(&value, 1, MPI_INT, dst, TAG_D, MPI_COMM_WORLD);
    }
  }
  else {
    MPI_Recv(&my_value, 1, MPI_INT, 0, TAG_D, MPI_COMM_WORLD, &status);
  }
  return my_value;
}

static void get_values(int rank, int procs, int value) {
  int        src;
  MPI_Status status;

  if (rank == 0) {
    printf("0: %d\n", value);
    for (src = 1; src < procs; src++) {
      MPI_Recv(&value, 1, MPI_INT, src, TAG_G, MPI_COMM_WORLD, &status);
      printf("%d: %d\n", src, value);
    }
  }
  else {
    MPI_Send(&value, 1, MPI_INT, 0, TAG_G, MPI_COMM_WORLD);
  }
}

static int transposition(int p, int rank, int procs, int value) {
  int        v, tmp;
  MPI_Status status;

  if (p < rank && p >= 0) {
    MPI_Recv(&v, 1, MPI_INT, p, TAG_S, MPI_COMM_WORLD, &status);
    if (v > value) {
      tmp = v;
      v = value;
      value = tmp;
    }
    MPI_Send(&v, 1, MPI_INT, p, TAG_S, MPI_COMM_WORLD);
  }
  else if (p > rank && p < procs) {
    MPI_Send(&value, 1, MPI_INT, p, TAG_S, MPI_COMM_WORLD);
    MPI_Recv(&value, 1, MPI_INT, p, TAG_S, MPI_COMM_WORLD, &status);
  }
  return value;
}

int main(int argc, char *argv[]) {
  int i, j, rank, procs, value;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &procs);

  value = distribute_values(rank, procs);
  for (i = 0; i < procs; i++) {
    j = 1 - 2 * ((i ^ rank) & 1);
    value = transposition(rank + j, rank, procs, value);
  }
  get_values(rank, procs, value);

  MPI_Finalize();
  return 0;
}
