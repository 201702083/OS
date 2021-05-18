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
        semaphore_wait(produce_ok); // ������ �ִٸ� ����, ������ block
        semaphore_wait(buffer_lock); // ���忡 �����ִٸ� ���� �� ���, ����ִٸ� block
        //
        if ( insertItem(item) == 0){
            printf("%s[PRODUCER_ERROR] insertItem Failed %s\n",T_RED,T_DEFAULT);
        }else{
            printf("%sproducer %d add CQ %d%s\n",T_YELLOW,producerID,item,T_DEFAULT);
        }
        semaphore_signal(consume_ok); // ������ �������� �ϳ� �þ��ٰ� �˸�
        semaphore_signal(buffer_lock); // ���� ��� ����
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
        semaphore_wait(consume_ok); // �������� �ִٸ� ����, ���ٸ� block
        semaphore_wait(buffer_lock); // ������ �����ִٸ� ���� �� ���, ����ִٸ� block

        if ( (item = takeItem()) == -1){
            printf("%s[CONSUMER_ERROR] takeItem Failed %s\n",T_RED,T_DEFAULT);
        }else{
            printf("%sconsumer %d take item from CQ %d%s\n",T_GREEN,consumerID,item,T_DEFAULT);

        }
        semaphore_signal(produce_ok); // ������ �ϳ� �þ��ٰ� �˸�
        semaphore_signal(buffer_lock); // ���� ��� ����

    }
    return 0;
}