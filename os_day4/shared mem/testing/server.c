#include <stdio.h>
#include <unistd.h> /* for fork() */
#include <sys/wait.h> /* for wait() */
#include <sys/types.h> /* for wait() kill(2)*/
#include <sys/ipc.h> /* for shmget() shmctl() */
#include <sys/shm.h> /* for shmget() shmctl() */
#include <signal.h> /* for signal(2) kill(2) */
#include <errno.h> /* for perror */
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SHM_KEY 0x1234
#define SHM_SIZE sizeof(struct task)

struct task {
    int status;
    char data[100];
    pid_t worker_pid;
};

struct task *sharedblock;
int shmid;

typedef void (*sighandler_t)(int);

void releaseSHM(int signum) {
    sharedblock->status = -1;
    shmdt(sharedblock); // detach and leave
    printf("Set the status to -1 and exit!!!\n");
    int status = kill(0, SIGKILL);
    if (status == 0) {
        fprintf(stderr, "kill successful.\n"); /* this line may not be executed :P WHY?*/
    } else if (status == -1) {
        perror("kill failed.\n");
        fprintf(stderr, "Cannot remove shared memory with id = %d.\n", shmid);
    } else {
        fprintf(stderr, "kill(2) returned wrong value.\n");
    }
}

int main() {
    sighandler_t shandler;
    /* install signal handler */
    shandler = signal(SIGINT, releaseSHM); /* should we call this separately in parent and child process */

    // Create shared memory
    shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    // Attach shared memory
    sharedblock = (struct task *)shmat(shmid, NULL, 0);
    if (sharedblock == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    sharedblock->status = 0;
    srand(time(NULL));

    while (1) {
        if (sharedblock->status == 0 || sharedblock->status==4) {
            int ranlen=rand()%100;
            
            for (int i = 0; i < ranlen; ++i) {
                sharedblock->data[i] = (char)((rand() % 26) + 65);
            }
            sharedblock->data[ranlen] = '\0';
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
