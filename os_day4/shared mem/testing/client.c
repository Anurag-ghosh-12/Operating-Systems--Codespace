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

#define SHM_KEY 0x1234
#define SHM_SIZE sizeof(struct task)

struct task {
    int status;
    char data[100];
    pid_t worker_pid;
};

int main() {
    key_t k=ftok("abcd",65);
    int shmid = shmget(k, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    struct task *sharedblock = (struct task *)shmat(shmid, NULL, 0);
    if (sharedblock == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    while (1) {
        if (sharedblock->status == -1) {
            break;
        }

        if (sharedblock->status == 1) {
            sharedblock->status = 2;
            sharedblock->worker_pid = getpid();

            // Compute the length of the null-terminated string in data[]
            printf("The received data is \t:%s\n",sharedblock->data);
            int length = strlen(sharedblock->data);
            sharedblock->data[0] = length; // Store the length in data[0]

            sharedblock->status = 3;
        }

        usleep(100000); // Sleep for 100ms 
    }

    shmdt(sharedblock); // Detach shared memory
    return 0;
}
