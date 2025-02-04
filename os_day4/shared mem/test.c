#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h> /* for fork() */
#include <sys/wait.h> /* for wait() */
#include <sys/types.h> /* for wait() kill(2)*/
#include <sys/ipc.h> /* for shmget() shmctl() */
#include <sys/shm.h> /* for shmget() shmctl() */
#include <signal.h> /* for signal(2) kill(2) */
#include <errno.h> /* for perror */
#include <stdlib.h>


#define SHM_SIZE sizeof(struct shared_data)

struct shared_data {
    int n;
    int parent_flag;
    int child_flag;
};

int factorial(int num) {
    if (num == 0 || num == 1) return 1;
    int result = 1;
    for (int i = 2; i <= num; ++i) {
        result *= i;
    }
    return result;
}

int main() {
    key_t key = ftok("shmfile", 65);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    int shmid = shmget(key, SHM_SIZE, IPC_CREAT|  0777 );
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    struct shared_data *shared = (struct shared_data *)shmat(shmid, NULL, 0);
    if (shared == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    shared->parent_flag = 0;
    shared->child_flag = 0;

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) 
    {
        while (1) {
            if (shared->parent_flag == 1) {
                int num = shared->n;
                printf("Child sensed a number: %d\n", shared->n);
                shared->n = factorial(num);
                shared->child_flag = 1;
                shared->parent_flag = 0;
            }
        }
    } else {
        srand(time(NULL));
        for (int i = 0; i < 10; ++i) {
            while (shared->child_flag == 1);

            int random_num = rand() % 10 + 1;
            shared->n = random_num;
            shared->parent_flag = 1;

            while (shared->child_flag == 0);

            printf("Parent received factorial: %d\n", shared->n);
            shared->child_flag = 0;
        }

        shmdt(shared);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}

