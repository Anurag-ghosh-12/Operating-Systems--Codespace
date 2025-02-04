#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "sharedmemory.h"
struct task * sharedblock;

int main() {
    struct task *sharedblock=attach_memory_block();
    if(sharedblock== NULL)
    {
    printf("Error: Could not get block!\n");
    return -1;
    }  
    srand(time(NULL));

    while (1) {
        if (sharedblock->status == -1) {
            break;
        }

        if (sharedblock->status == 1) {
            sharedblock->status = 2;
            sharedblock->worker_pid = getpid();

            int length = strlen(sharedblock->data);
            sharedblock->data[0] = length; 

            sharedblock->status = 3;
        }

        usleep(100000); 
    }

    shmdt(sharedblock); // Detach shared memory
    return 0;
}
