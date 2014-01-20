#define N 1000000
#define D 20
#define L 100
#define Q 100000
#define K 30

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

float get_float() {
  return (float)rand() / (float)RAND_MAX;
}

int main() {
  printf("%d %d %d %d %d\n", N, D, L, Q, K);

  srand(0);

  for (int i = 0; i < N; i++) {
    printf("%d ", rand() % L);
    for (int j = 0; j < D; j++) {
      printf("%f ", get_float());
    }
    printf("\n");
  }

  for (int i = 0; i < Q; i++) {
    for (int j = 0; j < D; j++) {
      printf("%f ", get_float());
    }
    printf("\n");
  }

  return 0;
}
