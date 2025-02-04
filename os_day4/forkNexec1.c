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
    printf("You entered: %s\n", str);
    }
    return 0;
}
