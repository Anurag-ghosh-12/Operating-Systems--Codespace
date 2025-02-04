#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdbool.h>
struct task {
      /* The server process puts a few integers in the array data which is to be sorted by one of the worker processes */
     char data[100]; /* The array to hold a null-terminated string put by the server process and used by one worker process to find the string-length and put it back here itself. */
     pid_t worker_pid; /* The pid of the worker process that is computing (or has computed)  the length of the string in array data. */
     int status; /* status = 0 means that at present there is nothing in the array data[ ] that needs to be worked on by a worker process. 
     status = 1 means that the server process has put some string in the array data[ ] that needs to be worked on by a worker process. 
     status = 2 means that a worker process having pid = worker_pid has started computing the length of the string in the array data[ ]. 
     status = 3 means that the worker process having pid = worker_pid has computed the length of the string in the array data[ ] and put the computed length in the space for the array data[] itself. 
     status = 4 means that the server process is using the "length" available in data[ ] computed by a worker process. status = -1 means that the server process has ended. */
};

//attach a shared memory block
//associated with filename
//create it if it doesn't exist
static key_t get_key();
struct task *attach_memory_block();
static int get_shared_block();
bool detach_memory_block(struct task *);
bool destroy_memory_block();

//all of the programs will sharing these values
#define BLOCK_SIZE 4096
#define FILENAME "writeshmem.c"
#endif
