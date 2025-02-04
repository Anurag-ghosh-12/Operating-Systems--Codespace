#include <stdio.h> /* needed also for perror() */
#include <errno.h> /* needed  for perror() */
#include <unistd.h> /* needed for execve() */
#include <string.h>
#include<stdlib.h>
int main(int argc, char *argv[]) {
       /* int status;
        char *myargv[]={"/bin/ls","-l","8086", NULL};
        printf("Inside the program\n");
//       int execve(const char *filename, char *const argv[], char *const envp[]);
        status = execve("/bin/ls", myargv, NULL);
        printf("Exited from program\n");//not printing this part
        if (status == -1) {
                perror ("Exec Fails: ");
        }*/
           int count=0;
           char *str="This is Earth in Solar System. ";
		
	    for(int i = 0; i < strlen(str); i++) {
		if(str[i] == ' ' && str[i+1] != ' ' && str[i+1] != '\0') {
		    count++;
		}
	    }
	    count++; // For the last word

	    printf("Number of words: %d\n", count);

	    char *myargv[count + 1];
	    char word[100];
	    int x = 0, w = 0;

	    for(int i = 0; i <= strlen(str); i++) {
		if(str[i] != ' ' && str[i] != '\0') {
		    word[w++] = str[i];
		} else {
		    if(w > 0) {
		        word[w] = '\0';
		        myargv[x] = (char *)malloc((w + 1) * sizeof(char));
		        strcpy(myargv[x], word);
		        x++;
		        w = 0;
		    }
		}
	    }
	    myargv[x] = NULL;

	    // Print words
	    for(int i = 0; i < count; i++) {
		printf("%s\t", myargv[i]);
		free(myargv[i]);
	    }
}
