#include<stdio.h>

void foo(void){
  printf("foo");
}

void foo2(void){
  printf("foo2");
}

void bar(void){
  foo();
}

void foo3(void){
  bar();
}
