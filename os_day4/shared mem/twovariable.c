#include <stdio.h>
#include <unistd.h> /* for fork() */
#include <sys/wait.h> /* for wait() */
#include <sys/types.h> /* for wait() kill(2)*/
#include <sys/ipc.h> /* for shmget() shmctl() */
#include <sys/shm.h> /* for shmget() shmctl() */
#include <signal.h> /* for signal(2) kill(2) */
#include <errno.h> /* for perror */
#include <stdlib.h>
#include <sharedmemory.h>
struct task * sharedblock;
typedef void (*sighandler_t)(int);

void releaseSHM(int signum) {
        block->status=-1;
        shmdt(sharedblock);//detach and leave
        
        status = kill(0, SIGKILL);
        if (status == 0) {
                fprintf(stderr, "kill susccesful.\n"); /* this line may not be executed :P WHY?*/
        } else if (status == -1) {
                perror("kill failed.\n");
                fprintf(stderr, "Cannot remove shared memory with id = %d.\n", shmid);
        } else {
                fprintf(stderr, "kill(2) returned wrong value.\n");
        }
}

int main(){
   sighandler_t shandler;
   /* install signal handler */
   // sighandler_t signal(int signum, sighandler_t handler);
   shandler =  signal(SIGINT, releaseSHM); /* should we call this seperately in parent and child process */
        
   struct task *sharedblock=attach_memory_block();
   if(sharedblock== NULL)
   {
    printf("Error: Could not get block!\n");
    return -1;
   }   
   sharedblock->status = 0;
   srand(time(NULL));

    while (1) {
        if (sharedblock->status == 0) {
            for (int i = 0; i < 99; ++i) {
                sharedblock->data[i] = (char)((rand() % 26) + 65);
            }
            sharedblock->data[99] = '\0';
            sharedblock->status = 1;
        }

        while (sharedblock->status != 3 && sharedblock->status != -1) {
            usleep(100000); // Sleep for 100ms
        }

        if (sharedblock->status == 3) {
            printf("The length of shared string was %d.\n", (int)strlen(sharedblock->data));
            printf("The length was calculated by %d.\n", sharedblock->worker_pid);
            sharedblock->status = 4;
        } else if (sharedblock->status == -1) {
            break;
        }
    }
    return 0;
}
