#include <stdio.h>
#include <unistd.h> /* for fork() */
#include <sys/wait.h> /* for wait() */
#include <sys/types.h> /* for wait() kill(2) */
#include <sys/ipc.h> /* for shmget() shmctl() */
#include <sys/shm.h> /* for shmget() shmctl() */
#include <signal.h> /* for signal(2) kill(2) */
#include <errno.h> /* for perror */
#include <stdlib.h>

#define MAX_DIM 10

// Creating shared memory
int shm_id;

int main() {
    int rows_A, cols_A, cols_B,rows_B;
    printf("Enter the dimensions of matrix A (rows columns): ");
    scanf("%d %d", &rows_A, &cols_A);

    printf("Enter dimensions of matrix B (rows columns): ");
    scanf("%d %d", &rows_B, &cols_B);
    if (cols_A <= 0 || rows_A <= 0 || cols_B <= 0 || rows_B!=cols_A) {
        fprintf(stderr, "Invalid matrix dimensions for multiplication.\n");
        exit(EXIT_FAILURE);
    }
    int matrix_A[MAX_DIM][MAX_DIM], matrix_B[MAX_DIM][MAX_DIM];
    printf("Enter the elements of matrix A (%d x %d):\n", rows_A, cols_A);
    for (int i = 0; i < rows_A; i++) {
        for (int j = 0; j < cols_A; j++) {
            scanf("%d", &matrix_A[i][j]);
        }
    }
    printf("Enter the elements of matrix B (%d x %d):\n", cols_A, cols_B);
    for (int i = 0; i < cols_A; i++) {//since rowsB=colsA
        for (int j = 0; j < cols_B; j++) {
            scanf("%d", &matrix_B[i][j]);
        }
    }
    shm_id = shmget(IPC_PRIVATE, (rows_A * cols_B) * sizeof(int), IPC_CREAT | 0777); 
    int *shared_matrix = shmat(shm_id, NULL, 0);
    
    // Create child processes to compute product matrix and wait for specific child process
    for (int i = 0; i < rows_A; i++) {
        for (int j = 0; j < cols_B; j++) {
            pid_t pid = fork();

            if (pid == -1) {
                perror("fork failed");
                exit(EXIT_FAILURE);
            } 
            else if (pid == 0) {  // Child process
                int element = 0;
                for (int k = 0; k < cols_A; k++) {
                    element += matrix_A[i][k] * matrix_B[k][j];
                }
                shared_matrix[i * cols_B + j] = element;
                exit(0);
            } else {  // Parent process
                int status;
                waitpid(pid, &status, 0);
                if (status == 0)
                    printf("Child process with pid %d completed successfully.\n", pid);           
            }
        }
    }
   
    printf("Product Matrix: \n");
    for (int i = 0; i < rows_A; i++) {
        for (int j = 0; j < cols_B; j++) {
            printf("%d ", shared_matrix[i * cols_B + j]);
        }
        printf("\n");
    }

    shmdt(shared_matrix);
    int status1 = shmctl(shm_id, IPC_RMID, NULL); /* IPC_RMID is the command for destroying the shared memory */
    if (status1 == 0) {
        fprintf(stderr, "Removed shared memory with id = %d.\n", shm_id);
    } else if (status1 == -1) {
        fprintf(stderr, "Cannot remove shared memory with id = %d.\n", shm_id);
    } else {
        fprintf(stderr, "shmctl() returned wrong value while removing shared memory with id = %d.\n", shm_id);
    }
    
    return 0;
}
