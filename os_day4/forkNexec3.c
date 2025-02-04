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
    printf("Enter a valid executable file name : ");
    
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
	      char *myargv[]={str,NULL};
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
	       if (WIFEXITED(s)) {
                       printf("Child exited normally with exit status %d.\n", WEXITSTATUS(s));
		    } 
               else if (WIFSIGNALED(s)) {
		      int sig = WTERMSIG(s);
		      printf("Child terminated abnormally by signal %d (%s).\n", sig, strsignal(sig));
		   } 
	       else {
		        printf("Child terminated abnormally for an unknown reason.\n");
		    }
	    }
	   printf("-----------------------------------\n");
    }
    return 0;
}
