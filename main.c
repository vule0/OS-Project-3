#include "readerwriter.c"


int main(){
    //create reader/writer lock
    rwlock_t reader_writer;
    rwlock_init(&reader_writer);
    //inputs and counters, opens file
    char line[50];
    int i;
    pthread_t threads[50];
    FILE* ptr = fopen("scenarios.txt", "r");
    while (!feof(ptr)){
        fscanf(ptr, "%s", line);
        //creating threads depending on the read character
        for (i = 0; i < strlen(line); i++){
            if (line[i] == 'r'){
                pthread_create(&threads[i], NULL, (void*) reading, (void*) &reader_writer);
            }
            else if (line[i] == 'w'){
                pthread_create(&threads[i], NULL, (void*) writing, (void*) &reader_writer);
            }
        }
        //joining all threads after completion
        for (i = 0; i < strlen(line); i++){
            pthread_join(threads[i], NULL);
        }
    }
    fclose(ptr);
    return 1;
}