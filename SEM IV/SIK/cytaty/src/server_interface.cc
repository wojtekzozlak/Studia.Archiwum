/* --Quotations storage protocol server shell--                        */
/*                                                                     */
/* Program forks into a shell instance and a server instance connected */
/* with pipes. The shell sends into the server a port number and       */
/* enters a interactive state.                                         */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sstream>
#include <algorithm>
#include "database.h"
using namespace std;

#define PERR_CHECK(x, y) if(-1 == (x)){ perror(y); exit(EXIT_FAILURE); };

int err_log = -1;
pid_t server;

/* Server related data. */
struct ServerData
{
  int in[2];
  int out[2];
  string program;
  uint16_t port;
  bool binded;
  string dbname;
};

static void catch_signal(int sig)
/* Signal, cleanup function. */
{
  if(-1 != err_log)
    close(err_log);
  printf("\nClosing...\n");
  if(SIGINT == sig)
  {
    kill(server, SIGINT);
    wait();
    exit(EXIT_SUCCESS);
  }
  else if(SIGCHLD == sig)
    exit(EXIT_FAILURE);
}

void trim(string& str)
/* Trims leading and tailing newlines from string. */
{
  string::size_type pos = str.find_last_not_of('\n');
  if(pos != string::npos) {
    str.erase(pos + 1);
    pos = str.find_first_not_of('\n');
    if(pos != string::npos) str.erase(0, pos);
  }
  else str.erase(str.begin(), str.end());
}

void comm_rebind(ServerData &data)
/* Tries to rebind server to another port. */
{
  uint8_t response;
  PERR_CHECK(write(data.in[1], &data.port, sizeof(data.port)), "rebind write");
  PERR_CHECK(read(data.out[0], &response, 1), "rebind read");
  data.binded = (1 == response);
  if(data.binded)
    printf("Server binded successfully.\n");
  else
    printf("Can't bind! See error log for details.\n");
  return ;
}

void comm_help()
/* Prints out available commands. */
{
  printf("Available commands:\n");
  printf("  help        - displays this help\n");
  printf("  status      - shows a server status\n");
  printf("  rebind PORT - changes a server port\n");
  printf("  add         - adds a quotation into a database\n");
  printf("  quit        - quits this program\n");
  printf("\n");
  return ;
}

void comm_status(ServerData &data)
/* Prints out a server status. */
{
  if(data.binded == 1)
    printf("Server is bined to a port %d\n", data.port);
  else
    printf("Server isn't binded to any port. Use 'rebind' to fix that.\n");
  return ;
}

void fork_out(ServerData &data)
/* Forks into a shell program and a server program. */
{
  switch(server = fork())
  {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);
      break;

    case 0:
      dup2(data.in[0], 0);
      dup2(data.out[1], 1);
      dup2(err_log, 2);
      PERR_CHECK(execlp(data.program.c_str(), data.program.c_str(), NULL), "execl");
      break;

    default:
      comm_rebind(data);
      break;
  }
  return ;
}

void comm_add(ServerData &data)
/* Adds a quotation into a database. */
{
  string line;
  printf("Please type a quotation to add.\n");
  printf("Insert EOF (usually Ctrl^D) at an empty line to end an input.\n");
  string quotation;
  while(!cin.eof() && !cin.fail())
  {
    printf(": ");
    getline(cin, line);
    quotation.append(line).append("\n");
  }
  if(cin.eof())
    cin.clear();
  else if(cin.fail())
    printf("Err: Reading from input failed!\n");
  printf("\n");

  QuotationsDB db(data.dbname.c_str());
  trim(quotation);
  db.insert_quotation(quotation);
  printf("Quotation added successfully.\n");
  return ;
}

int main(int argc, char *argv[])
{
  if(2 > argc)
  {
    fprintf(stderr, "Usage: %s port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* Setting up cleanup handler. */
  if(SIG_ERR == signal(SIGCHLD, catch_signal) ||
     SIG_ERR == signal(SIGINT, catch_signal))
    {
      perror("Unable to change signal handler\n");
      exit(EXIT_FAILURE);
    }

  ServerData server_data;
  server_data.port = atoi(argv[1]);
  server_data.program = string("./server");
  server_data.binded = false;
  string err_log_file("err.log");

  /* Opening a server pipe. */
  PERR_CHECK(pipe(server_data.in), "pipe");
  PERR_CHECK(pipe(server_data.out), "pipe");
  err_log = open(err_log_file.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
  PERR_CHECK(err_log, "open error log");

  printf("Starting server at port %d, with error log in '%s'\n", server_data.port,
         err_log_file.c_str());
  fork_out(server_data);

  /* Main shell loop. */
  string line, command;
  while(!cin.eof() && !cin.fail())
  {
    printf(">> ");
    getline(cin, line);
    stringstream line_str(line);
    line_str >> command;

    if(0 == command.compare("help"))
      comm_help();
    else if(0 == command.compare("rebind"))
    {
      line_str >> server_data.port;
      if(line_str.fail())
      { printf("Error: Invalid argument\n"); continue; }
      comm_rebind(server_data);
    }
    else if(0 == command.compare("status"))
      comm_status(server_data);
    else if(0 == command.compare("add"))
      comm_add(server_data);
    else if(0 == command.compare("quit"))
      break;
    else
    {
      printf("Error: Unknown command.\n");
      printf("       Try 'help' to see available commands.\n");
    }
    command.clear();
  }

  /* Clean termination. */
  catch_signal(SIGINT);
  return 0;
}
