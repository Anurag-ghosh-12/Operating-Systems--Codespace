#include<stdio.h>
#include <unistd.h>
int main()
{
	pid_t p;
	printf("Hello World\n");//by the parent process only
	p=fork();//in parent the return value is pid of child whereas in the child its 0

	if(p==0)
	{
		//excuted only by child 
		printf("Child process\n");
	}
	else
	{//executed by parent only
		printf("Parent process\n");
	}
	printf("Hello World( Universal)\n");//executed by both
}

