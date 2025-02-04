#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
//Anurag Ghosh(2022CSB101)
void reverse_string(char *str) {
    int n = strlen(str);
    for (int i = 0; i < n / 2; i++) {
        char temp = str[i];
        str[i] = str[n - i - 1];
        str[n - i - 1] = temp;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Enter more strings please!");
        return 1;
    }
    //i started from 1 because argv[0] is the name of the program executed.
    for (int i = 1; i < argc; i++) {
        pid_t p = fork();
        if (p < 0) {
            //on failure -1 is returned.
            perror("Fork error observed!");
	    //perror prints error message details according to errno
            exit(1);
        } else if (p == 0) {
            // Child process entered
            reverse_string(argv[i]);
            printf("Reversed string %d: %s\n", i, argv[i]);
            exit(0);
        }
	else
	{
		int *s;
		wait(s);
	}
    }
    return 0;
}
