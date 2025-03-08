#include <stdio.h>
#include <unistd.h>  // fork()
#include <sys/wait.h> // wait()
#include <sys/types.h> // pid_t
#include <sys/ipc.h> // shmget(), shmctl()
#include <sys/shm.h> // shmget(), shmctl()
#include <stdlib.h>

int main() {
    key_t k = ftok("matmult.c", 65);
    int *arr;
    int shmid = shmget(IPC_PRIVATE, 6 * sizeof(int), IPC_CREAT | 0777);

    arr = (int *)shmat(shmid, NULL, 0);
    arr[5]=0;//status for parent 
    pid_t p = fork();
    if (p < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    else if (p == 0) 
    {  // Child process
        while(arr[5]==0);
        printf("Printing the shared memory in child process:\n");
        for (int i = 0; i < 6; i++) {
            printf("%d\t", arr[i]);
        }
        printf("\n");
        shmdt(arr);
        exit(0);
    }
    else {
    //parent exclusive
        printf("Enter 6 numbers: ");
        for (int i = 0; i < 6; i++) {
            scanf("%d", &arr[i]);
        }
        int status;
        waitpid(p, &status, 0);
        printf("Child process %d completed successfully.\n", p);
        shmdt(arr);
        shmctl(shmid, IPC_RMID, NULL);
    }
    return 0;
}

