#include <stdio.h>
#include "mpi.h"

#define MAX_MSG_LEN   80
#define TAG           50

int main(int argc, char *argv[]) {
  int        len, rank, procs, src, dst, tag;
  char       msg[MAX_MSG_LEN];
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &procs);

  printf("I am process %d.\n", rank);
  tag = TAG;
  if (rank != 0) {
    len = snprintf(msg, MAX_MSG_LEN, "Greetings from process %d!", rank);
    len = len >= MAX_MSG_LEN ? MAX_MSG_LEN : len + 1;
    dst = 0;
    MPI_Send(msg, len, MPI_CHAR, dst, tag, MPI_COMM_WORLD);
  }
  else {
    for (src = 1; src < procs; src++) {
      MPI_Recv(msg, MAX_MSG_LEN, MPI_CHAR, src, tag, MPI_COMM_WORLD, &status);
      printf("%.*s\n", MAX_MSG_LEN, msg);
    }
  }

  MPI_Finalize();
  return 0;
}
