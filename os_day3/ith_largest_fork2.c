#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <limits.h>

int find_ith_largest(int *arr, int n, int i) {
    int max = INT_MIN;
    int min = INT_MAX;
    for (int j = 0; j < n; j++) {
        if (arr[j] > max) max = arr[j];
        if (arr[j] < min) min = arr[j];
    }

    int range = max - min + 1;
    int *count = (int *)calloc(range, sizeof(int));
    if (count == NULL) {
	    perror("calloc");
        exit(1);
    }
    for (int j = 0; j < n; j++) {
        count[arr[j] - min]++;
    }
    int total = 0;
    for (int j = range - 1; j >= 0; j--) {
        total += count[j];
        if (total >= i) {
            free(count);
            return j + min;
        }
    }

    free(count);
    return -1;
}

int main() {
    int n;
    printf("Enter the number of elements: ");
    scanf("%d", &n);

    int *arr = (int *)malloc(n * sizeof(int));
    if (arr == NULL) {
        perror("malloc");
        exit(1);
    }

    printf("Enter the elements: ");
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }
    
    pid_t childprocess[n];
    for (int i = 0; i < n; i++) {
        pid_t p = fork();
        if (p < 0) {
		//error handling of fork
            perror("fork");
            exit(1);
        } else if (p == 0) {
            // Child process
            int ith_largest = find_ith_largest(arr, n, i + 1);
            //printf("The %dth largest number is: %d\n", i + 1, ith_largest);
     
            exit(ith_largest);
        }
	else
	{
		childprocess[i]=p;
		//wait(s);
	}
    }
    int result[n];
    for(int i=0;i<n;i++)
    {
        int status = 0;
        pid_t finish_pid = waitpid(childprocess[i], &status, 0);

        if (finish_pid < 0) {
            perror("waitpid");
            exit(1);
        }

        if (WIFEXITED(status)) {
            result[i] = WEXITSTATUS(status);
            printf(" %d\n", result[i]);
        } 
     }
    free(arr);
    return 0;
}
