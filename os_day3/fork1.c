#include <stdio.h>

#include <unistd.h>

int main() {

                 printf("Hello World by the parent process(exclusive)!!\n");  

                 fork();                            /* Executed by parent process only*/

                 printf("Hello World 2!!\n");      /* Executed by both  parent and child processes .*/

}
