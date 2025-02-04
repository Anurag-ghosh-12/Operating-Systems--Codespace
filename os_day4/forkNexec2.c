#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <limits.h>
#include <string.h>

int main() {
   while(1){
    
    char str[100];
    printf("Enter a string : ");
    fgets(str, sizeof(str), stdin);
  
    pid_t p = fork();    
	 if(p<0)
	    {
	    perror("Fork failed");
	    exit(1);
            }
	 else if(p==0)
	    {
	      printf("Inside the child process\n");
	      //child process area 
	      printf("The string is  ... %s\n", str);
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
