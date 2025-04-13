#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_CMD_SIZE 1024
#define MAX_ARG_SIZE 50
void execute_command(char *command);
void parse_command(char *command, char **args);
void execute_internal_command(char *command);
void execute_external_command(char *command);
int is_internal_command(char *command);
void handleMultipleCommands(char *command);
int main() {
  char command[MAX_CMD_SIZE];
  int flag = 0;
  do {
    printf("mysh>");
    fflush(stdout);
    
    if (!fgets(command, sizeof(command), stdin)) {
      flag = 1; // exit
    }
    command[strcspn(command, "\n")] = '\0';
    
    printf("------------------------------------\n");
    printf("Executing ... \n");
    if (strncmp(command, "exit", 4) == 0) {
      flag = 1; // user wants to exit
    }
    if (flag != 1) {
      execute_command(command);
    }
    printf("------------------------------------\n");
  } while (flag == 0);
}

/*id handleMultipleCommands(char *command) {
    char *subcommands[MAX_ARG_SIZE];
    char *delimiters = ";|&";
    char *token = strtok(command, delimiters);
    int i = 0;

    while (token != NULL) {
        subcommands[i++] = token;
        token = strtok(NULL, delimiters);
    }
    subcommands[i] = NULL;

    for (int j = 0; subcommands[j] != NULL; j++) {
        char *args[MAX_ARG_SIZE];
        parse_command(subcommands[j], args);

        if (args[0] == NULL) {
            continue;
        }

        if (is_internal_command(subcommands[j]==0){
            execute_internal_command(args);
        }
    }
}
*/

void execute_command(char *command) {
  if (strchr(command, '|') != NULL || strstr(command, "&&") != NULL || strstr(command, "||") != NULL) {
    //ndleMultipleCommands(command);
  } else if (is_internal_command(command)) {
    execute_internal_command(command);
  } else {
    execute_external_command(command);
  }
}

int is_internal_command(char *command) {
  if (strncmp(command, "cd", 2) == 0 || strncmp(command, "exit", 4) == 0 ||
      strncmp(command, "clear", 4) == 0 || strncmp(command, "pwd", 3) == 0) {
    return 1; // Internal command
  }
  return 0; // Not an internal command
}

void execute_internal_command(char *command) {
  if (strncmp(command, "cd", 2) == 0) {
    char *path = strtok(command + 3, " ");
    if (chdir(path) != 0) {
      perror("cd failed");
    }
  } else if (strncmp(command, "exit", 4) == 0) {
    exit(0);
  } else if (strncmp(command, "clear", 4) == 0) {
    system("clear");
  } else if (strncmp(command, "pwd", 3) == 0) {
    char cwd[MAX_CMD_SIZE];
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
  }
  return;
}
void execute_external_command(char *command) {
  char *myargv[MAX_ARG_SIZE];
  parse_command(command, myargv);
  pid_t p = fork();

  if (p < 0) {
    perror("Fork failed");
    exit(1);
  } else if (p == 0) { // Child process
    
    if (execvp(myargv[0], myargv) == -1) {
      printf("Bad Command !!!\n");
      exit(1);
    }
  } else { // Parent process
    int s;
    wait(&s);
  }
}
void parse_command(char *command, char **args) {
  int i = 0;
  char *token = strtok(command, " ");
  while (i < MAX_ARG_SIZE && token != NULL) {
    args[i++] = token;
    token = strtok(NULL, " ");
  }
  args[i] = NULL;
}
