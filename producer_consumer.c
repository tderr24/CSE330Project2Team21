#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/vmalloc.h>

#define AUTHOR "CSE330 Group 21"
MODULE_AUTHOR(AUTHOR)

static int buffSize;
static int prod;
static int cons;
static uid_t uuid;

//0644 permission allows user to read and write
module_param(buffSize, int, 0644);
module_param(prod, int, 0644);
module_param(cons, int, 0644);
module_param(uuid, int, 0644);

//initialize threads here
struct task_struct *producer_thread;
struct tast_struct *buffer;
struct task_struct *consumer_thread;
struct tast_struct *p;

static int processes = 0;
static int producerIndex = 0;

//initialize semaphores
struct semaphore empty;
struct semaphore mutex;
struct semaphore full;

static int init_producer_consumer(void) {
    //this is the function that will run threads
    if(prod == 1) {
        //creates buffer after defining bufferSize and confirming there is a producer
        buffer = vmalloc(buffSize*sizeof(struct tastk_struct));
        //initialize semaphores
        sema_init(&empty, buffSize);
        sema_init(&mutex, 0);
        sema_init(&full, 1);
        producer_thread = kthread_run(kthread_producer, NULL, "producer");
        //if there is a producer, must check if there is at least one consumer
        //char *consName;
        char consName[MAX];
        if(cons > 0) {
            for(int i = 1; i < cons; i++) {
                strcat(strcpy(consName, "Consumer- "), i);
                consumer_thread = kthread_run(kthread_consumer, NULL, consName);
            }
        }
    }
    //returns 0 if there isnt any producers
    return 0;
}

static int kthread_producer(void *arg) {
    for_each_process(p) {
        if(p->cred->uid.val == uuid) {
            //checks if there is a consumer to read anything
            if(cons == 0) {
                break;
            }
            //idk what should go here
            if(down_interruptible(??)) {
                break;
            }
            buffer[producerIndex] = *p;
            processes++;
            //allows producer to continue iterating through the buffer without exceeding the max size
            producerIndex = (producerIndex+1) % buffSize;
            printk("[producer] Produced Item#-%d at buffer index:%d for PID:%d\n", processes, producerIndex, buffer[producerIndex].pid);
            up(??);
        }
        
    }
}

static int kthread_consumer(void *arg) {
    while(!kthread_should_stop()) {
        //calculating elapsed time should go here 
        printk("[Consumer-i] Consumed...");
    }
}

static void exit_producer_consumer(void) {
    //exit the module
    printk("The total elapsed time of all processes for...")
}
