#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main() {
    FILE *fp = fopen("abc.txt", "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    //P1 is parent 
    char buffer[100];
    printf("-----------------------------------------------\n");
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("P1 reads before fork done: %s\n", buffer);
    }
    printf("-----------------------------------------------\n");
    //fclose(fp);-- if here parent close the file -> all the following
    // read operations has failed!!
    // Fork the process
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
    }
    else if (pid == 0) 
     {
        // P2 is child process
        printf("P2 started:\n");
        printf("-----------------------------------------------\n");
        if (fgets(buffer, sizeof(buffer), fp) != NULL) {// child starts reading where parent left previously
            printf("P2 reads: %s\n", buffer);
            printf("-----------------------------------------------\n");
        }
        else
        {
          printf("Could not read by P2(end of file/ or P1 has closed file.)\n ");
          printf("-----------------------------------------------\n");
        }
        fclose(fp); // P2 closed the file, child closing the file does not affect the parent's reading condition
        printf("P2 closed the file.\n");
        printf("-----------------------------------------------\n");
    } 
    else 
    {
        //Parent executes here
        wait(NULL); 
        printf("P1 resumed...\n");
        printf("-----------------------------------------------\n");
        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            printf("P1 read: %s\n", buffer);
            printf("-----------------------------------------------\n");
        } else {
            printf("P1 could not read: End of file.\n");
        }
        fclose(fp);
        printf("P1 closed the file.\n");
        printf("-----------------------------------------------\n");
    }

    return 0;
}

