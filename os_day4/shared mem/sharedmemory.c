#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "sharedmemory.h"

#define IPC_RESULT_ERROR (-1)
#define SHM_SIZE sizeof(struct task)

static key_t get_key(){
  key_t key=ftok("shmfile",65);
  return key;
}
static int get_shared_block()
{
    key_t key;
    // Request a key
    key=ftok("shmfile", 65);
    // The key is linked to a filename so that other programs can access it
    if (key == IPC_RESULT_ERROR)
    {
        return IPC_RESULT_ERROR;
    }

    // get the shared block = create if it doesent exist
    return shmget(key, SHM_SIZE, IPC_CREAT|  0777 );
    // load or create if not exist 0644 is like permission
}

struct task *attach_memory_block()
{
    int shared_block_id = get_shared_block();
    
    struct task *shared = (struct task *)shmat(shared_block_id, NULL, 0);
    if (shared == (void *)-1) {
        perror("shmat");
        exit(1);
    }
    //shared->key=get_key();
    shared->status = -1;//initially
    
    if (shared_block_id == IPC_RESULT_ERROR)
    {
        return NULL;
    }

    return shared;
}

bool detach_memory_block( struct task *block)
{
    return (shmdt(block) != IPC_RESULT_ERROR);
}

bool destroy_memory_block()
{
    int shared_block_id = get_shared_block();

    if (shared_block_id == IPC_RESULT_ERROR)
    {
        return NULL;
    }
    return (shmctl(shared_block_id, IPC_RMID, NULL) != IPC_RESULT_ERROR);
}

