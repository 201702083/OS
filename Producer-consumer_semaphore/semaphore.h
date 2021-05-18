#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <mach/semaphore.h>
#include <mach/mach_traps.h>
#include <mach/mach_init.h>
#include <mach/mach_error.h>
#include <mach/task.h>
#include "cq.h"


#define PRODUCER_COUNT 5
#define CONSUMER_COUNT 2

#define PRODUCE_TIME 6
#define CONSUMER_TIME 15

kern_return_t produce;
kern_return_t consume;
kern_return_t buffer;
mach_port_t self;
semaphore_t consume_ok ;
semaphore_t produce_ok ;
semaphore_t buffer_lock ;

int producerCount;
int consumerCount;

void init();
void* producer(void* );
void* consumer(void* );

#endif