#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> /* for thread functions */
#include <errno.h>

#define nproducer 2 /* number of producers */
#define nconsumer 3 /* number of consumers */
#define INIT_CQUEUE(X) cqueue X = {.front=-1, .rear=-1, .size=0}
typedef struct cqueue{
   int arr[10];
   int producerno;
   int consumerno;
   int front;
   int rear;
}cqueue;

cqueue *q;
INIT_CQUEUE(q);

void * producer(void * param);
void * consumer(void * param);
pthread_mutex_t p_mutex;
pthread_mutex_t c_mutex;

int isFull(cqueue * q)
{
 if((*q)->front==(*q->rear +1)%()
}
int main()
{ 
    int i;
    pthread_t ptids[nproducer]; 
    pthread_t ctids[nconsumer]; 
    int status;

    pthread_attr_t attr; /*Set of thread attributes required to be passed in pthread_create() */
    /* Initialize the mutexes */
    //int pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr);
    status = pthread_mutex_init(&p_mutex, NULL);
    status = pthread_mutex_init(&c_mutex, NULL);
    /* Block the consumers - no data is produced - producer()s however can proceed */
    /* int pthread_mutex_lock(pthread_mutex_t *mutex); */
    status =  pthread_mutex_lock(&c_mutex); //Lock the consumer 


