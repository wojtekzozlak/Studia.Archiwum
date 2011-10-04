gcc $1.c -o $1 -Wall -O2 -D WAITING_FOR=$2 -pthread -levent
