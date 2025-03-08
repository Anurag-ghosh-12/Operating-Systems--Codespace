#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

union semun {
    int              val;    /* value for SETVAL */
    struct semid_ds buf;    /* buffer for IPC_STAT, IPC_SET */
    unsigned short  array;  /* array for GETALL, SETALL */
    struct seminfo  __buf;  /* buffer for IPC_INFO (Linux-specific) */
};

/* Function to print usage information */
void usage(const char *prog) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s <file_name> <project_id> create <semnum>\n", prog);
    fprintf(stderr, "  %s <file_name> <project_id> set <semnum> <sem_val>\n", prog);
    fprintf(stderr, "  %s <file_name> <project_id> get [<semnum>]\n", prog);
    fprintf(stderr, "  %s <file_name> <project_id> inc <semnum> <val>\n", prog);
    fprintf(stderr, "  %s <file_name> <project_id> dcr <semnum> <val>\n", prog);
    fprintf(stderr, "  %s <file_name> <project_id> rm\n", prog);
    fprintf(stderr, "  %s <file_name> <project_id> listp [<semnum>]\n", prog);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    char *file_name = argv[1];
    int proj_id = atoi(argv[2]);  /* convert project id to integer */
    char *command = argv[3];
    key_t key = ftok(file_name, proj_id);
    if (key == -1) {
        perror("ftok failed");
        exit(EXIT_FAILURE);
    }
    int semid; // semaphore set id

    /*
     * Command: create
     */
    if (strcmp(command, "create") == 0) {
        if (argc != 5) {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
        int nsems = atoi(argv[4]);//number of semaphores
        semid = semget(key, nsems, IPC_CREAT | 0666);
        if (semid == -1) {
            perror("semget (create) failed");
            exit(EXIT_FAILURE);
        }

        /* Initialized all semaphores to 0 */
        unsigned short *initvals = malloc(nsems * sizeof(unsigned short));
        if (initvals == NULL) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < nsems; i++) {
            initvals[i] = 0;
        }
        union semun arg;
        arg.array = initvals;
        if (semctl(semid, 0, SETALL, arg) == -1) {
            perror("semctl SETALL failed");
            free(initvals);
            exit(EXIT_FAILURE);
        }
        free(initvals);
        printf("Semaphore set created with key %d and %d semaphore(s), initialized to 0.\n", key, nsems);
    }
    /*
     * Command: set
     */
    else if (strcmp(command, "set") == 0) {
        if (argc != 6) {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
        int semnum = atoi(argv[4]);
        int sem_val = atoi(argv[5]);
        semid = semget(key, 1, 0);
        if (semid == -1) {
            perror("semget (set) failed");
            exit(EXIT_FAILURE);
        }
        union semun arg;
        arg.val = sem_val;
        if (semctl(semid, semnum, SETVAL, arg) == -1) {
            perror("semctl SETVAL failed");
            exit(EXIT_FAILURE);
        }
        printf("Semaphore %d set to value %d.\n", semnum, sem_val);
    }
    /*
     * Command: get
     */
    else if (strcmp(command, "get") == 0) {
        semid = semget(key, 1, 0);
        if (semid == -1) {
            perror("semget (get) failed");
            exit(EXIT_FAILURE);
        }
        if (argc == 4) {
            union semun arg;
            int nsems = sem_ds.sem_nsems;
            unsigned short *vals = malloc(nsems * sizeof(unsigned short));
            if (vals == NULL) {
                perror("malloc failed");
                exit(EXIT_FAILURE);
            }
            arg.array = vals;
            if (semctl(semid, 0, GETALL, arg) == -1) {
                perror("semctl GETALL failed");
                free(vals);
                exit(EXIT_FAILURE);
            }
            printf("Semaphore set values:\n");
            for (int i = 0; i < nsems; i++) {
                printf("  Semaphore %d: %d\n", i, vals[i]);
            }
            free(vals);
        } else if (argc == 5) {
            int semnum = atoi(argv[4]);
            int val = semctl(semid, semnum, GETVAL, 0);
            if (val == -1) {
                perror("semctl GETVAL failed");
                exit(EXIT_FAILURE);
            }
            printf("Semaphore %d value: %d\n", semnum, val);
        } else {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    /*
     * Command: inc
     */
    else if (strcmp(command, "inc") == 0) {
        if (argc != 6) {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
        int semnum = atoi(argv[4]);
        int inc_val = atoi(argv[5]);
        semid = semget(key, 1, 0);
        if (semid == -1) {
            perror("semget (inc) failed");
            exit(EXIT_FAILURE);
        }
        struct sembuf sop;
        sop.sem_num = semnum;
        sop.sem_op = inc_val;  /* positive value: increment */
        sop.sem_flg = SEM_UNDO;
        if (semop(semid, &sop, 1) == -1) {
            perror("semop (inc) failed");
            exit(EXIT_FAILURE);
        }
        int new_val = semctl(semid, semnum, GETVAL, 0);
        if (new_val == -1) {
            perror("semctl GETVAL after inc failed");
            exit(EXIT_FAILURE);
        }
        printf("Semaphore %d incremented by %d. New value: %d\n", semnum, inc_val, new_val);
    }
    /*
     * Command: dcr
     */
    else if (strcmp(command, "dcr") == 0) {
        if (argc != 6) {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
        int semnum = atoi(argv[4]);
        int dec_val = atoi(argv[5]);
        semid = semget(key, 1, 0);
        if (semid == -1) {
            perror("semget (dcr) failed");
            exit(EXIT_FAILURE);
        }
        struct sembuf sop;
        sop.sem_num = semnum;
        sop.sem_op = -dec_val;  /* negative value: decrement */
        sop.sem_flg = SEM_UNDO;
        if (semop(semid, &sop, 1) == -1) {
            perror("semop (dcr) failed");
            exit(EXIT_FAILURE);
        }
        int new_val = semctl(semid, semnum, GETVAL, 0);
        if (new_val == -1) {
            perror("semctl GETVAL after dcr failed");
            exit(EXIT_FAILURE);
        }
        printf("Semaphore %d decremented by %d. New value: %d\n", semnum, dec_val, new_val);
    }
    /*
     * Command: rm
     */
    else if (strcmp(command, "rm") == 0) {
        if (argc != 4) {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
        semid = semget(key, 1, 0);
        if (semid == -1) {
            perror("semget (rm) failed");
            exit(EXIT_FAILURE);
        }
        if (semctl(semid, 0, IPC_RMID) == -1) {
            perror("semctl (rm) failed");
            exit(EXIT_FAILURE);
        }
        printf("Semaphore set with key %d removed.\n", key);
    }
    else {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    return 0;
}

