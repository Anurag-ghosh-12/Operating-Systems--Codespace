#include<unistd.h>
#include <stdio.h>
#include<stdlib.h>
int main()
{       
        pid_t p;
        for(int i=0;i<5;i++)
        {       
                        p = fork();
                        if (p == 0) {
                               printf("Child process! %d\n", i+1);

                               exit(0);

                        } else {
                               printf("Parent process!",get_pid());
			 
                        }      
        }               
}
