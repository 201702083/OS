#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define Q_SIZE 10
#define P_CNT 6
#define C_CNT 15

pthread_cond_t buffer = PTHREAD_COND_INITIALIZER;
pthread_cond_t item = PTHREAD_COND_INITIALIZER;
pthread_mutex_t buffer_lock = PTHREAD_MUTEX_INITIALIZER;

int CQ[Q_SIZE];
int front = 0;
int rear = 0;
int CQ_cnt = 0;

int count = 0 ;
int first = 1;
int pc = 1;
int cc = 1;

int getQ(){
	int output = 99;
	if ( CQ_cnt == 0 ) {
		printf("Queue is empty.\n");
		return -1;
	} else {
		output = CQ[rear];
		rear = (rear + 1 ) %10;
		CQ_cnt--;
		return output;
	}
}


int addQ(int input){
	if ( CQ_cnt == 10 ){
		printf("Queue is full.\n");
		return -1;
	} else {
		CQ[front] = input;
		front = (front + 1) % 10;
		CQ_cnt++;
		return 1;
	}
}	

void *producer(void *arg){
	int i;
	int id;
	int input;

	id = pc++;
	for( i = 0 ; i < P_CNT; i ++){
		input = random()%100;
		usleep(input);
		pthread_mutex_lock(&buffer_lock); // lock
		while(CQ_cnt == Q_SIZE ) pthread_cond_wait(&buffer,&buffer_lock); 
		// 공간이 생길 떄가지 계속 unlock 상태 유지		
		// 공간이 생겨야 아래 코드로 넘어감

		addQ(input);
		printf("producer %d addQ %d\n",id, input);
		count++;
		pthread_cond_signal(&item); // 아이템을 넣었다고 알림 
		pthread_mutex_unlock(&buffer_lock);
		
	}
	return 0;
}

void *consumer( void *arg ){
	int i;
	int id;
	int output;
	id = cc++;
	for ( i = 0 ; i < C_CNT; i ++){
		
		usleep(random()%100);
		pthread_mutex_lock(&buffer_lock); // lock 

                while(CQ_cnt == 0) pthread_cond_wait(&item,&buffer_lock); 
		// 아이템이 들어올 때 까지 계속 unlock 상태 유지
		// 아이템이 들어와야 아래 코드로 넘어감

		output = getQ();
		if ( output != -1){
			printf("Consumer %d getQ %d\n", id,output);
		}
		count++;
		pthread_cond_signal(&buffer); // 넣을 공간이 생겼다고 알림 
		pthread_mutex_unlock(&buffer_lock); // unlock
	}
	return 0;
	
}

int main(){
	pthread_t producer_thread[5];
	int producer_id[5];
	pthread_t consumer_thread[2];
	int consumer_id[2];

	int status ;
	int i;
	
	srand(time(NULL));

	//create  p thread
	for (  i = 0 ; i < 5 ; i ++ ){
		if (( producer_id[i] = pthread_create(&producer_thread[i],NULL,producer,NULL)) < 0 ) {
			exit(0);
		}
	}
	
	//create  c thread
	for (  i = 0 ; i < 2 ; i ++ ){
                if (( consumer_id[i] = pthread_create(&consumer_thread[i],NULL,consumer,NULL)) < 0 ) {
           	     exit(0);
	        }
	}

	//start threads
	pthread_join( producer_thread[0], (void **)&status);
	pthread_join( producer_thread[1], (void **)&status);
	pthread_join( producer_thread[2], (void **)&status);
	pthread_join( producer_thread[3], (void **)&status);
	pthread_join( producer_thread[4], (void **)&status);
	pthread_join( consumer_thread[0], (void **)&status);
	pthread_join( consumer_thread[1], (void **)&status);
	printf("Total count : %d\n",count);	
	return 0;
}

