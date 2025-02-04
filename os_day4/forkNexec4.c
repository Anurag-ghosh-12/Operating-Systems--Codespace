#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int status;
        /*char *myargv[]={"/bin/ls", NULL};
        printf("Inside the program\n");
        //int execve(const char *filename, char *const argv[], char *const envp[]);
        status = execve("/bin/ls", myargv, NULL);
        if (status == -1) {
        perror ("Exec Fails: ");// SAMPLE*/
   
   while(1){
    printf("-----------------------------------\n");
    char str[100];
    printf("Enter a valid executable file name with switches (seperate by spaces) : ");
    
    fgets(str, sizeof(str), stdin);
    str[strcspn(str, "\n")] = 0;//removing newline
    
    pid_t p = fork();    
	 if(p<0)
	    {
	    perror("Fork failed");
	    exit(1);
            }
	 else if(p==0)
	    {
	       int count=0;
	      //char *myargv[]={str,NULL};
	      //like /bin/ls -l 8086 
	       for(int i = 0; i < strlen(str); i++) {
		if(str[i] == ' ' && str[i+1] != ' ' && str[i+1] != '\0') {
		    count++;
		}
	    }
	    count++; // For the last word

	    printf("Number of words entered by user: %d\n", count);

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

	      printf("Inside the child process\n");
	      //child process area 
	      printf("Executing ... %s\n", str);
	      printf("------------------------------------\n");
	      status = execve(myargv[0], myargv, NULL);
	      if (status == -1) 
	      {
                //execve failing
                 perror ("Exec Fails: ");
                 exit(1);
	      }
	      exit(0);//normal execution
	    }
	  else
	    {  int s;
	       wait(&s);
	       if(WIFEXITED(s))
	       {
	         printf("Normal termination of the child process.\n");
	       }
	       else
	       {
	          printf("Abnormal execution of the childprocess.\n");
	       }
	     }
	     printf("-----------------------------------\n");
    }
    return 0;
}
