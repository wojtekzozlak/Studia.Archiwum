#include<iostream>
#include <fstream>
#include<stdlib.h>
#include<ncurses.h>
#include <sys/stat.h> 
using namespace std;



bool FileExists(string strFilename) {
  struct stat stFileInfo;
  bool blnReturn;
  int intStat;

  // Attempt to get the file attributes
  intStat = stat(strFilename.c_str(),&stFileInfo);
  if(intStat == 0) {
    // We were able to get the file attributes
    // so the file obviously exists.
    blnReturn = true;
  } else {
    // We were not able to get the file attributes.
    // This may mean that we don't have permission to
    // access the folder which contains this file. If you
    // need to do that level of checking, lookup the
    // return values of stat which will give you
    // more details on why stat failed.
    blnReturn = false;
  }
  
  return(blnReturn);
}

int main()
{
  if( !FileExists("zad") )
  {
    printf("blad: nie znaleziono pliku zadania \"zad\"\n");
    return -1;
  }

  if( !FileExists("conf") )
  {
    printf("blad: nie znaleziono pliku ustawien \"conf\"\n");
    return -1;
  }

  char s2[100];
  char s[100];
  int ilosc;
  char pattern_in[500];
  char pattern_stdout[500];
  char pattern_errout[500];

  ifstream set("conf");
  set.getline(s, 255);
  sscanf(s, "%d", &ilosc);
  set.getline(pattern_in, 255);
  set.getline(pattern_stdout, 255);
  set.getline(pattern_errout, 255);



  int state;


  for(int i=1; i<=ilosc; i++)
  {
    sprintf(s2, pattern_in, i);
    sprintf(s, "./zad < %s 1> std.out 2> err.out", s2);
    state = system(s);

    sprintf(s2, pattern_stdout, i);
    sprintf(s, "cmp -s %s std.out", s2);
    state = system(s);
    printf("Test %d, stdout: ", i);
    if(state == 0) printf("\033[32m[OK]\033[37m\n");
    else printf("\033[31m[FAIL]\033[37m\n");

    sprintf(s2, pattern_errout, i);
    sprintf(s, "cmp -s %s err.out", s2);
    state = system(s);
    printf("Test %d, stderr: ", i);
    if(state == 0) printf("\033[32m[OK]\033[37m\n");
    else printf("\033[31m[FAIL]\033[37m\n");

    printf("--\n");

  }

  system("rm std.out -f");
  system("rm err.out -f");

  return 0;
}
