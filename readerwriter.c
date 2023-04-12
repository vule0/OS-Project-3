#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

typedef struct _rwlock_t { // Struct for reader/writer
    sem_t rw_mutex; //lock for exclusive writing and concurrent reading
    sem_t r_mutex; //lock for reading
    int readerCount; //number of read operations occuring
    sem_t fairness;  //controls access to resources
} rwlock_t;

void rwlock_init(rwlock_t *lock) { 
    lock->readerCount = 0;
    //initializing all locks to 1
    sem_init(&lock->rw_mutex, 0, 1);
    sem_init(&lock->r_mutex, 0, 1);
    sem_init(&lock->fairness, 0, 1);
}

void reading_writing(int read_write){ //Console output function
    int x = 0, i, j, T = rand()%10000;
    for (i = 0; i < T; i++){
        for (j = 0; j < T; j++){
            x = i * j;
        }
    }
    if (read_write){
        printf("Writer is writing\n");
    }
    else{
        printf("Reader is reading\n");
    }
}

void rwlock_acquire_readlock(rwlock_t *lock) { 
    // delay the calling until semaphore count is > 0, then decrement it (locking it)
    sem_wait(&lock->fairness);
    sem_wait(&lock->r_mutex);
    lock->readerCount++;
    if (lock->readerCount == 1){ //locks rw_mutex, no writes allowed
        sem_wait(&lock->rw_mutex);
    }
    // increment the semaphore (unlocking it)
    sem_post(&lock->r_mutex);
    sem_post(&lock->fairness);
}

void rwlock_release_readlock(rwlock_t *lock) {
    sem_wait(&lock->r_mutex);
    lock->readerCount--;
    if (lock->readerCount == 0){ // if last reader, unlocks the rw_mutex, allowing for write operation
        sem_post(&lock->rw_mutex);
    }
    sem_post(&lock->r_mutex);
}

void rwlock_acquire_writelock(rwlock_t *lock) {
    sem_wait(&lock->fairness);      //wait for rw_mutex, then locks it
    sem_wait(&lock->rw_mutex);
    sem_post(&lock->fairness);
}

void rwlock_release_writelock(rwlock_t *lock) {
    sem_post(&lock->rw_mutex);      //unlocks rw_mutex
}

void *reading(rwlock_t *lock){      //function for reading process. each operation acquires the r_mutex and releases
    rwlock_acquire_readlock(lock);
    printf("Created reader\n");
    reading_writing(0);
    rwlock_release_readlock(lock);
    printf("Finished reading\n");
}


void *writing(rwlock_t *lock){  //function for write process. each operation waits for rw_mutex, then releases
    rwlock_acquire_writelock(lock);
    printf("Created writer\n");
    reading_writing(1);
    rwlock_release_writelock(lock);
    printf("Finished writing\n");
}


