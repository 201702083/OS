#include "cq.h"
#include "semaphore.h"
#include <mach/semaphore.h>
#include <mach/mach_traps.h>
#include <mach/mach_init.h>
#include <mach/mach_error.h>

int main(int argc, char* argv[]){

    pthread_t producer_thread[PRODUCER_COUNT];
    int producer_id[PRODUCER_COUNT];
    int producer_state[PRODUCER_COUNT];
    pthread_t consumer_thread[CONSUMER_COUNT];
    int consumer_id[CONSUMER_COUNT];
    int consumer_state[CONSUMER_COUNT];



    int i ;
    initCQ(); // cq 积己
    init(); // 技付器绢 积己



    for (i = 0 ; i < PRODUCER_COUNT; i ++){
        if((producer_id[i] = pthread_create(&producer_thread[i], NULL, producer, NULL))){
            printf("producer %d create error\n", i);
            exit(0);
        }
    }

    for (i = 0 ; i < CONSUMER_COUNT; i ++){
        if((consumer_id[i] = pthread_create(&consumer_thread[i], NULL, consumer, NULL))){
            printf("consumer %d create error\n" , i);
            exit(0);
        }
    }

    for ( i = 0 ; i < PRODUCER_COUNT; i ++) {
        pthread_join(producer_thread[i], (void**)&producer_state[i]);

    }
    for ( i = 0 ; i < CONSUMER_COUNT; i++) {
        pthread_join(consumer_thread[i],(void**)&consumer_state[i]);
    }
    // semaphore_destroy(&buffer_lock);
    // semaphore_destroy(&produce_ok);
    // semaphore_destroy(&consume_ok);

}