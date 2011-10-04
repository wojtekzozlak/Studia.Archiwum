#include "page_sim.c"
#include <pthread.h>
#include <stdio.h>

#define PAGE_SIZE 2
#define MEM_SIZE 1
#define ADDR_SIZE 4
#define OPERATIONS 10

int k;

void* watek(void *attr)
{
  printf("zrobilem watek\n");
  int l;
  for(l = 0; l < 100; l++)
  {
  
  unsigned gdzie = rand() % (PAGE_SIZE * ADDR_SIZE);
  uint8_t what = (uint8_t) rand();
  if(rand() % 3 == 0)
  {
    printf("chcialbym wczytać spod %d\n\n", gdzie);
    get_page(gdzie, &what);
    printf("wczytałem %d\n\n", what);
  }
  else
  {
    printf("chcialbym zapisać w %d\n\n", gdzie);
    set_page(gdzie, what);
    printf("zapisałem %d\n", what);
  }
  
  }
  
  k--;
  
  return ;
}

int main()
{
  srand(time(0));
  int i;
  uint8_t ret;
  if(page_sim_init(PAGE_SIZE, MEM_SIZE, ADDR_SIZE, OPERATIONS, NULL) == 0)
    printf("init successful\n");
 
  pthread_t tab[100];
  pthread_attr_t attr;
  pthread_attr_init(&attr);

  k = 100;
 
  for(i = 0; i < 100; i++)
  {
    pthread_create(tab, &attr, watek, NULL);
  }
 
  sleep(10);
  close(data.file);
  unlink(data.file_name);
  
  printf("zostało : %d\n\n", k);
  return 0;
}
