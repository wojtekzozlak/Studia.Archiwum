#include "../pagesim.h"
#include <pthread.h>
#include <stdio.h>

#define PAGE_SIZE 512
#define MEM_SIZE 24
#define ADDR_SIZE 512
#define OPERATIONS 8

#define ITERATIONS 1

#define THREADS ADDR_SIZE

int k;
uint8_t nums[THREADS];
uint8_t wynik[ADDR_SIZE * PAGE_SIZE];

FILE *llog;

void callback(int op, int arg1, int arg2)
{
  if(op == 1)
    fprintf(llog, "ODWOŁANIE DO STRONY %d\n", arg1);
  else if(op == 2)
    fprintf(llog, "INICJACJA ZAPISU STRONY %d (ramka %d)\n", arg1, arg2);
  else if(op == 3)
    fprintf(llog, "KONIEC ZAPISU STRONY %d (ramka %d)\n", arg1, arg2);
  else if(op == 4)
    fprintf(llog, "INICJACJA ODCZYTU STRONY %d (ramka %d)\n", arg1, arg2);
  else if(op == 5)
    fprintf(llog, "KONIEC ODCZYTU STRONY %d (ramka %d)\n", arg1, arg2);
  else if(op == 6)
    fprintf(llog, "KONIEC OPERACJI NA STRONIE %d (ramka %d)\n", arg1, arg2);
}

void* watek(void *attr)
{
  uint8_t *x = (uint8_t*) attr;
  
  printf("zrobilem watek\n");
  int i, j;

  for(j = 0; j < ITERATIONS; j++)
  {
    for(i = PAGE_SIZE * (*x); i < PAGE_SIZE * ((*x) + 1); i++)
    {
      usleep(rand() % 500);
      if(page_sim_set(i, *x) != 0)
        printf("set err : %d\n", errno);
      printf("zapisuję %d w %d\n", (*x), i);
    }
  }
  
  k--;
  printf("=== koniec wątku ===\n");
  
  return ;
}

int main()
{
  srand(time(0));
  llog = fopen("log", "w+");
  int i;
  uint8_t ret;
  if(page_sim_init(PAGE_SIZE, MEM_SIZE, ADDR_SIZE, OPERATIONS, callback) == 0)
    printf("init successful\n");
 
  pthread_t tab[THREADS];
  pthread_attr_t attr;
  pthread_attr_init(&attr);

  k = THREADS;
 
  for(i = 0; i < THREADS; i++)
  {
    nums[i] = i;
    pthread_create(tab, &attr, watek, &nums[i]);
  }
  
  while(k != 0)
  {
    usleep(100);
  }
 
 
  int z = k;
  
  for(i = 0; i < ADDR_SIZE * PAGE_SIZE; i++)
    page_sim_get(i, &wynik[i]);
    
  for(i = 0; i < ADDR_SIZE * PAGE_SIZE; i++)
    printf("%d ", wynik[i]);
  printf("\n");
  

  page_sim_end();  
  
  printf("zostało : %d\n\n", z);
  return 0;
}
