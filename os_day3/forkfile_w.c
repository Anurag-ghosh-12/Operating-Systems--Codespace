#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    FILE *fp = fopen("abc.txt", "w");
    if (fp == NULL) {
        perror("Error opening file");
    }

    fprintf(fp, "P1 writes before fork\n");

    // Fork the process
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
    } else if (pid == 0) {
        // Child process (P2)
        printf("P2 started to write ...\n");
        fprintf(fp, "P2 writes after fork\n");
        printf("P2 closed the file.\n");
        fclose(fp); // P2 closes the file
    } else {
        // Parent process (P1)
        wait(NULL); // Wait for the child process to finish
        fprintf(fp, "P1 writes after P2 finishes\n");
        fclose(fp); // P1 closes the file
        printf("P1 closed the file.\n");
    }

    return 0;
}

