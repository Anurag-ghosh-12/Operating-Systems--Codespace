/* sem1.c */
#include <stdio.h>
#include <stdlib.h> /* for exit(3) */
#include <string.h>
#include <sys/ipc.h>   /* for semget(2) ftok(3) semop(2) semctl(2) */
#include <sys/sem.h>   /* for semget(2) semop(2) semctl(2) */
#include <sys/types.h> /* for semget(2) ftok(3) semop(2) semctl(2) */
#include <unistd.h>    /* for fork(2) */
#define NO_SEM 3
#define P(s) semop(s, &Pop, 1);
#define V(s) semop(s, &Vop, 1);

struct sembuf Pop;
struct sembuf Vop;

int main(int argc, char *argv[]) {

  int num_of_sem = 0;
  
  union semun {
    int val;               /* Value for SETVAL */
    struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
    struct seminfo *__buf; /* Buffer for IPC_INFO (Linux-specific) */
  } setvalArg;

  setvalArg.val = 1;
  int semid;
  if (argc < 5) {
    perror("Used less arguments\n");
    exit(EXIT_FAILURE);
  }

  char *str = argv[3];
  int projid = atoi(argv[2]);
  key_t mykey = ftok(argv[1], projid);
  if (mykey == -1) {
    perror("ftok() failed");
    exit(1);
  }

  if (strcmp(str, "create") == 0) {
    //       int semget(key_t key, int nsems, int semflg);
    semid = semget(mykey, atoi(argv[4]), IPC_CREAT | 0777);
    num_of_sem = atoi(argv[4]);
    if (semid == -1) {
      perror("semget() failed");
      exit(1);
    }
    printf("Semaphore set created successfully\n\n");
    printf("%s %s %s create %s will create a key on the file "
           "/home/manas/sem.txt and project id 4; then it will a semaphore set "
           "having 3 semaphores on that key.",
           argv[0], argv[1], argv[2], argv[4]);
  } else if (strcmp(str, "set") == 0) {
    int semnumth = atoi(argv[4]);
    int setflag = atoi(argv[5]);
    setvalArg.val = setflag;

    semid = semget(mykey, atoi(argv[4]), IPC_CREAT | 0777);
    int status = semctl(semid, semnumth, SETVAL, setvalArg);

    if (status == -1) {
      perror("semctl() failed\n");
      exit(1);
    }
    printf("Sets %d to be  the value of the %d th semaphore of the semaphore "
           "set identified by the key derived (by ftok()) from %s and %d\n",
           setflag, semnumth, argv[1], projid);
  } 
  else if (strcmp(str, "get") == 0) {
    semid = semget(mykey, atoi(argv[4]), IPC_CREAT | 0777);
    int status = semctl(semid, atoi(argv[4]), GETVAL);
    if (status == -1) {
      perror("semctl() failed\n");
      exit(1);
    }
    if (argc < 5) {
      // get values of all the semaphores
      int sem_values[num_of_sem]; // Array to store semaphore values

      // Get semaphore values using GETALL
      if (semctl(semid, 0, GETALL, sem_values) == -1) {
        perror("semctl GETALL failed");
        exit(EXIT_FAILURE);
      }
      printf("The semaphore values are :\t");
      for (int i = 0; i < num_of_sem; i++) {
        printf("%d",sem_values[i]);
      }
    }
  }
  else if(strcmp(argv[3],"inc")==0)
  {
     int val=atoi(argv[5]);
     int semnum=atoi(argv[4]);
     semid = semget(mykey, atoi(argv[4]), IPC_CREAT | 0777);
    int current_val = semctl(semid, semnum, GETVAL);
    if (current_val == -1) {
        perror("semctl GETVAL");
        exit(EXIT_FAILURE);
    }
    
    printf("Current value of semaphore %d: %d\n", semnum, current_val);

    struct sembuf op = {semnum, val, 0};
    if (semop(semid, &op, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }

    current_val = semctl(semid, semnum, GETVAL);
    if (current_val == -1) {
        perror("semctl GETVAL");
        exit(EXIT_FAILURE);
    }
    
    printf("Updated value of semaphore %d: %d\n", semnum, current_val);
  }

  return 0;
}
