#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define MAX_THREADS 10

// Mutex & Condition Variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_produce = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_consume = PTHREAD_COND_INITIALIZER;

// Circular Queue
typedef struct {
    int buffer[BUFFER_SIZE];
    int front;
    int rear;
} CircularQueue;

CircularQueue queue = {{0}, 0, 0};

// Thread Counters
int producer_count = 0, consumer_count = 0;
pthread_t producers[MAX_THREADS], consumers[MAX_THREADS];

// Helper Functions
int isFull() {
    return (queue.rear + 1) % BUFFER_SIZE == queue.front;
}

int isEmpty() {
    return queue.front == queue.rear;
}

int noOfFreeSpacesQ() {
    return (BUFFER_SIZE - 1) - ((queue.rear - queue.front + BUFFER_SIZE) % BUFFER_SIZE);
}

// Queue Operations
void enqueue(int item) {
    queue.buffer[queue.rear] = item;
    queue.rear = (queue.rear + 1) % BUFFER_SIZE;
}

int dequeue() {
    int item = queue.buffer[queue.front];
    queue.front = (queue.front + 1) % BUFFER_SIZE;
    return item;
}

// Producer Function
void* producer(void* arg) {
    int id = *(int*)arg;
    free(arg);

    while (1) {
        pthread_mutex_lock(&mutex);

        // Wait if the queue is full
        while (isFull()) {
            printf("[Producer %d] Queue Full! Waiting...\n", id);
            pthread_cond_wait(&cond_produce, &mutex);
        }

        // Produce an item and add it to the queue
        int item = rand() % 100;
        enqueue(item);
        printf("[Producer %d] Produced: %d\n", id, item);

        // Signal the consumer that an item is available
        pthread_cond_signal(&cond_consume);
        pthread_mutex_unlock(&mutex);
        usleep(500000);  // Slow down output for readability
    }
    return NULL;
}

// Consumer Function
void* consumer(void* arg) {
    int id = *(int*)arg;
    free(arg);

    while (1) {
        pthread_mutex_lock(&mutex);

        // Wait if the queue is empty
        while (isEmpty()) {
            printf("[Consumer %d] Queue Empty! Waiting...\n", id);
            pthread_cond_wait(&cond_consume, &mutex);
        }

        // Consume an item from the queue
        int item = dequeue();
        printf("[Consumer %d] Consumed: %d\n", id, item);

        // Signal the producer that a space is available
        pthread_cond_signal(&cond_produce);
        pthread_mutex_unlock(&mutex);
        usleep(500000);
    }
    return NULL;
}

// Function to Add a Producer
void addProducer() {
    if (producer_count >= MAX_THREADS) {
        printf("Max Producer Limit Reached!\n");
        return;
    }
    int* id = malloc(sizeof(int));
    *id = producer_count + 1;
    pthread_create(&producers[producer_count++], NULL, producer, id);
    printf("Producer %d added.\n", *id);
}

// Function to Add a Consumer
void addConsumer() {
    if (consumer_count >= MAX_THREADS) {
        printf("Max Consumer Limit Reached!\n");
        return;
    }
    int* id = malloc(sizeof(int));
    *id = consumer_count + 1;
    pthread_create(&consumers[consumer_count++], NULL, consumer, id);
    printf("Consumer %d added.\n", *id);
}

// Function to Remove a Producer
void removeProducer() {
    if (producer_count == 0) {
        printf("No Producers to Remove!\n");
        return;
    }
    pthread_cancel(producers[--producer_count]);
    printf("Producer removed.\n");
}

// Function to Remove a Consumer
void removeConsumer() {
    if (consumer_count == 0) {
        printf("No Consumers to Remove!\n");
        return;
    }
    pthread_cancel(consumers[--consumer_count]);
    printf("Consumer removed.\n");
}

// Manager Thread
void* manager(void* arg) {
    int choice;
    while (1) {
        printf("\n1. Add Producer\n2. Add Consumer\n3. Remove Producer\n4. Remove Consumer\n5. Exit\nEnter Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addProducer(); break;
            case 2: addConsumer(); break;
            case 3: removeProducer(); break;
            case 4: removeConsumer(); break;
            case 5: exit(0);
            default: printf("Invalid Choice!\n");
        }
    }
    return NULL;
}

// Main Function
int main() {
    pthread_t manager_thread;

    // Start Manager Thread
    pthread_create(&manager_thread, NULL, manager, NULL);
    pthread_join(manager_thread, NULL);

    return 0;
}
