#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h> 

#include "fifo.h"
#include "err.h"

#define LINE_SIZE 1024

static int prompt_user(const char *msg, char *buf, int buf_size)
{
  char line [LINE_SIZE];
  int line_len;
    
  for (;;) {
    printf("%s: ", msg);
    if (fgets (line, LINE_SIZE, stdin) == 0) {
      printf ("\n");
      exit (0);
    }
	
    line_len = strlen (line);
    if (line [line_len - 1] != '\n') {
      printf (" > Line too long or end of input\n");
      continue;
    }
    line [-- line_len] = 0;
    if (line_len >= buf_size) {
      printf (" > Response too long\n");
      continue;
    }
    if (line_len == 0){
      printf (" > Value required\n");
      continue;
    }
    strncpy (buf, line, buf_size);
    return line_len;
  }
}

int my_open(char *file_name, int flags, char *err_msg, char *ok_msg)
{
  int fd;
  if ((fd = open(file_name, flags)) == -1)
    perror(err_msg);
  else 
    printf(ok_msg);
  return fd;
}

int main ()
{
  char cmd[LINE_SIZE], buf[LINE_SIZE + 1];
  char *fifo_file;
  int count, flags, fd = -1;
  
  if ((fifo_file = getenv(FIFONAME_ENV)) == 0)
    fatal("missing %s environment variable", FIFONAME_ENV); 
  
  for (;;) {
    prompt_user ("Command (? for help)", cmd, sizeof (cmd));
    if (strlen (cmd) != 1) {
      printf (" > One letter only\n");
      continue;
    }
    switch (cmd [0]) {
      case '?':
        printf(" > c  create fifo\n");
	printf(" > i  open fifo to read\n");
	printf(" > j  open fifo to read  (O_NDELAY)\n");
	printf(" > o  open fifo to write\n");
	printf(" > p  open fifo to write (O_NDELAY)\n");
	printf(" > w  write to fifo\n");
	printf(" > r  read from fifo\n");
	printf(" > s  on/off flag O_NDELAY (default off)\n");
	printf(" > e  close fifo\n");
	printf(" > d  delete fifo\n");
	printf(" > q  quit\n");
	
	break;
    case 'c':
      if (mknod(fifo_file, S_IFIFO | 0666, 0) != 0)
	perror("Can't create fifo");
      else 
	printf(">OK create fifo %s\n", fifo_file);
      break;
    
    case 'o':
      if (fd != -1)
        close(fd);
      fd = my_open(fifo_file, O_WRONLY,
		 "Can't open fifo to write", ">OK open to write\n");
      break;

    case 'p':
      if(fd != -1)
        close(fd);
      fd = my_open(fifo_file, O_WRONLY | O_NDELAY,
		 "Can't open fifo to write", ">OK open to write\n");
      break;
    
    case 'i':
      if (fd != -1) 
        close(fd);
      fd = my_open(fifo_file, O_RDONLY,
		 "Can't open fifo to read", ">OK open to read\n");
      break;
    
    case 'j':
      if (fd != -1)
        close(fd);
      fd = my_open(fifo_file, O_RDONLY | O_NDELAY,
		 "Can't open fifo to read", ">OK open to read\n");
      break;
    
    case 'w':
      count = prompt_user ("Message", buf, sizeof (buf));
      
      if (write(fd, buf, count) != count)
	perror("Can't write to fifo");
      else 
	printf(">OK write\n");
      break;
    
    case 'r':
      switch (count = read(fd, buf, sizeof(buf) - 1)) {
        case -1:
	  perror("Can't read from fifo");
	  break;
        case 0:
	  printf(">end of file\n");
	  break;
        default:
	  buf[count] = 0;
	  printf(">Read %d bytes : %s\n", count, buf);
      }
      break;
    
    case 's':
      if ((flags = fcntl(fd, F_GETFL)) == -1)
	perror("Can't get flags");
      else
	if (flags & O_NDELAY){
	  if (fcntl(fd, F_SETFL, flags & ~O_NDELAY) == -1)
	    perror("Can't set flags");
	  printf(">OFF O_NDELAY\n");
	} else {
	  if (fcntl(fd, F_SETFL, flags | O_NDELAY) == -1)
	    perror("Can't set flags");
	  printf(">ON O_NDELAY\n");
	}
      break;
    
    case 'e':
      if (close(fd) != 0)
	perror("Can't close fifo");
      else 
	printf(">OK close fifo\n");
      break;
    
    case 'd': 
      if (unlink(fifo_file) != 0)
	perror("Can't delete fifo");
      else 
	printf(">OK delete fifo\n");
      break;   
    
    case 'q':
      fprintf (stderr, " > Exiting...\n");
      return 0;
    default:
      fprintf (stderr, " > Unknown command - use ? for help\n");
    }
  }
}
