#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
using namespace std;

void empty(char a)
{
  printf("let %c = empty;;\n", a);
}

void is_empty(char a)
{
  printf("print_string ( string_of_bool ( is_empty %c ) ^ \"\\n\" );;\n", a);
}

void add(char a, int x, int y)
{
  printf("let %c = add (%d, %d) %c ;;\n", a, x, y, a);
}

void remove(char a, int x, int y)
{
  printf("let %c = remove (%d, %d) %c ;;\n", a, x, y, a);
}

void mem(char a, int x)
{
  printf(" print_string (\"%d - \") ;;\n", x);
  printf(" print_string (string_of_bool (mem (%d) %c) ) ;;\n", x, a);
  printf(" print_string \"\\n\" ;; \n");
}

void elements(char a)
{
  printf("print_string (\"-- elem\\n\") ;;\n");
  printf("List.fold_left (fun x y -> print_pair y) () (elements %c);;\n", a);;
  printf("print_string \"\\n\" ;;\n");
}

void below(char a, int x)
{
  printf("print_int (below (%d) %c) ;\n", x, a);
  printf("print_string \"\\n\" ;;\n");
}

void split(char a, int x)
{
  printf("print_string (\"-- split (%d) \\n\") ;;", x); 
  printf("let (x, y, z) = split (%d) %c ;; \n", x, a);
  printf(" print_string (\"wieksze: \") ;; \n");
  elements('x');
  printf("print_string (\"mniejsze: \") ;; \n");
  elements('z');
  printf("print_string (\"jest? : \") ;; \n");
  printf("print_string ( (string_of_bool y) ^ \"\\n\\n\" ) ;;\n\n" );
}

void add_test(char a, int size, int mx, int len)
{
  printf(" print_string(\"--ADD_TEST -- %c ; %d ; %d ; %d --\\n\") ;;\n", a, size, mx, len);
  for(int i=0; i<size; i++)
  {
    int x = (rand()%mx)*pow(-1, rand()%2);
    int y = x + (rand()%len);
    add(a, x, y);
  }
  elements(a);
  printf("print_string(\"--DONE--\\n\\n\\n\");; \n\n");
}

void remove_test(char a, int size, int mx, int len)
{
  printf("print_string(\"--REMOVE_TEST -- %c ; %d ; %d ; %d -- \\n\") ;;\n", a, size, mx, len);
  for(int i=0; i<size; i++)
  {
    int x = (rand()%mx)*pow(-1,rand()%2);
    int y = x + (rand()%len);
    remove(a, x, y);
  }
  elements(a);
  printf("print_string(\"--DONE--\\n\\n\\n\");; \n\n");
}

void below_test(char a, int size, int mx)
{
  printf("print_string(\"--BELOW_TEST -- %c ; %d ; %d -- \\n\") ;;\n" , a, size, mx);
  for(int i=0; i<size; i++) below(a, (rand()%mx)*pow(-1, rand()%2));
  printf("print_string(\"--DONE--\\n\\n\\n\");; \n\n");
}

void mem_test(char a, int size, int mx)
{
  printf("print_string(\"--MEM_TEST -- %c ; %d ; %d -- \\n\") ;;\n" , a, size, mx);
  for(int i=0; i<size; i++) mem(a, (rand()%mx)*pow(-1, rand()%2));
  printf("print_string(\"--DONE--\\n\\n\\n\");; \n\n");
}

void split_test(char a, int size, int mx)
{
  printf("print_string(\"--SPLIT_TEST -- %c ; %d ; %d -- \\n\") ;;", a, size, mx);
  for(int i=0; i<size; i++) below(a, (rand()%mx)*pow(-1, rand()%2));
  printf("print_string(\"--DONE--\\n\\n\\n\");;");
}

void iter_test(char a){
  printf("print_string(\"iter: \");;\n");
  printf("iter (fun x -> print_pair x) %c;;\n", a);
  printf("print_string(\"\\n\\n\");;\n\n");
}

void fold_test(char a){
  printf("print_string(\"fold: \");;\n");
  printf("print_int ( fold (fun x il -> print_pair x ; il+1 ) %c 0) ;;\n", a);
  printf("print_string(\"\\n\\n\");;\n\n");
}

int main()
{
  printf("open ISet;;\n");
//  printf("open List;;\n\n");
  
  printf("let print_pair (x,y) =\n");
  printf("print_string \"[\";\n");
  printf("print_int x;\n");
  printf("print_string \" \";\n");
  printf("print_int y;\n");
  printf("print_string \"] \";;\n");
  
  srand(time(NULL));
  
  
for(int i=0; i<20; i++)
{  
  empty('a');
  add_test('a', 3000, 50000, 15);
  fold_test('a');
  iter_test('a');
  mem_test('a', 500, 50000);
  below_test('a', 500, 50000);
  split_test('a', 500, 50000);
  remove_test('a', 2000, 50000, 200);
}  
  return 0;
}