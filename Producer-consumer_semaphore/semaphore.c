#include "semaphore.h"

void init(){
    producerCount = 0;
    consumerCount = 0;

     self = mach_task_self();
     produce = semaphore_create(self, &produce_ok, SYNC_POLICY_FIFO, PRODUCE_ONCE);
     consume = semaphore_create(self, &consume_ok, SYNC_POLICY_FIFO, 0);
     buffer = semaphore_create(self, &buffer_lock,SYNC_POLICY_FIFO,1);
    if ( produce != KERN_SUCCESS){
        printf("%s[ERROR] Cannot init semaphore : producer_ok \n" , T_RED);
        exit(0);
    }
    if ( consume != KERN_SUCCESS){
        printf("%s[ERROR] Cannot init semaphore : consumer_ok \n" , T_RED);
        exit(0);
    }
    if ( buffer != KERN_SUCCESS){
        printf("%s[ERROR] Cannot init semaphore : buffer_lock \n" , T_RED);
        exit(0);
    }
}

void *producer(void *args){
    int i;
    int producerID = ++producerCount;
    int item;

    for ( i = 0 ; i < PRODUCE_TIME ; i ++){
        item = (random()%100) + producerID;
        usleep(item);

        //entry section
        semaphore_wait(produce_ok); // 공간이 있다면 시작, 없으면 block
        semaphore_wait(buffer_lock); // 담장에 열려있다면 시작 후 잠금, 잠겨있다면 block
        //
        if ( insertItem(item) == 0){
            printf("%s[PRODUCER_ERROR] insertItem Failed %s\n",T_RED,T_DEFAULT);
        }else{
            printf("%sproducer %d add CQ %d%s\n",T_YELLOW,producerID,item,T_DEFAULT);
        }
        semaphore_signal(consume_ok); // 공간에 아이템이 하나 늘었다고 알림
        semaphore_signal(buffer_lock); // 담장 잠금 해제
    }
    return 0;

}

void *consumer (void *args){
    int i;
    int consumerID = ++consumerCount;
    int time;
    for ( i = 0 ; i < CONSUMER_TIME ; i ++){
        int item;
        time = (random()%100) + consumerID;
        usleep(time);

            //entry section
        semaphore_wait(consume_ok); // 아이템이 있다면 시작, 없다면 block
        semaphore_wait(buffer_lock); // 담장이 열려있다면 시작 후 잠금, 잠겨있다면 block

        if ( (item = takeItem()) == -1){
            printf("%s[CONSUMER_ERROR] takeItem Failed %s\n",T_RED,T_DEFAULT);
        }else{
            printf("%sconsumer %d take item from CQ %d%s\n",T_GREEN,consumerID,item,T_DEFAULT);

        }
        semaphore_signal(produce_ok); // 공간이 하나 늘었다고 알림
        semaphore_signal(buffer_lock); // 담장 잠금 해제

    }
    return 0;
}