#include <stdio.h>

#include <unistd.h>

int main() {
                 pid_t p;
		 printf("Enter two numbers\n");
		 int a,b;
		 scanf("%d%d",&a,&b);


                 p = fork();

                 if (p == 0) {
                 //child execution
		     int c=a+b;
                     printf("The sum calculated from child is %d\n",c);
                   //  printf("The numbers are swapped in child\n");
               	     
                 } else {
                      printf("The entered numbers are (from parent) %d,%d\n",a, b);
                      int d=a-b;
                      printf("The difference is (from parent) %d\n",d);		      
		 }

                printf("The variables (common section) display %d,%d\t",a,b);
}
