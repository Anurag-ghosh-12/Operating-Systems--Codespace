
#include <stdio.h>

#include <unistd.h>

int main() {
                 pid_t p;

                 int i;

                 for (i=0; i < 5; i++) {

                         p = fork();
			 printf("[%d] [%d] i=%d\n", getppid(), getpid(), i);

                 }
}
