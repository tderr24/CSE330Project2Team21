#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/vmalloc.h>

#define AUTHOR "CSE330 Group 21"
//Group 21 Submission
MODULE_AUTHOR(AUTHOR);

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
struct task_struct *buffer;
struct task_struct *consumer_thread;
struct task_struct *p;

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
        buffer = vmalloc(buffSize*sizeof(struct task_struct));
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
            if(down_interruptible(&mutex)) { // has to be a semaphore that's all I know
                break;
            }
	    if(down_interruptible(&empty)) {
            	break;
	    }
            buffer[producerIndex] = *p;
            processes++;
            //allows producer to continue iterating through the buffer without exceeding the max size
            producerIndex = (producerIndex+1) % buffSize;
            printk("[producer] Produced Item#-%d at buffer index:%d for PID:%d\n", processes, producerIndex, buffer[producerIndex].pid);
            up(&mutex);
	    up(&full);
        }
        
    }
}


/*
    function _G.Text.formatTime(lap)

	    local min = math.floor(lap/60)
	    local sec = math.floor(lap)-60*min
	    local milisec = math.floor((lap-sec-min*60)*1000+0.5)

	    if min < 1 then
		    min = "00:"
	    elseif min > 0 and min < 10 then
		    min = "0"..min..":"
	    else
		    min = min..":"
	    end
	    if sec < 10 then
		    sec = "0"..sec
	    end
	    if milisec < 100 and milisec >= 10 then
		    milisec = "0"..milisec
	    elseif milisec < 10 then
		    milisec = "00"..milisec
	    end

	    local output = min..sec.."."..milisec

	    return output

    end
*/

// Generic time formatting function I ripped from my game and made into C lol
char* format_time(unsigned long lap) {
    unsigned long min = lap / 60; // decimals don't matter because it will be truncated
    unsigned long sec = lap - 60 * min;
    unsigned long milisec = (lap - sec - min * 60) * 1000;

    char* output = kmalloc(sizeof(char) * 9, GFP_KERNEL); // kmalloc is a contiguous memory allocation for small - medium size kernel level memory allocation & does not fragment kernel memory pool

    if (min < 1) {
        sprintf(output, "00:%02lu.%03lu", sec, milisec);
    }
    else if (min > 0 && min < 10) {
        sprintf(output, "0%lu:%02lu.%03lu", min, sec, milisec);
    }
    else {
        sprintf(output, "%lu:%02lu.%03lu", min, sec, milisec);
    }

    return output;
}

int consumer_thread_number = 1; // 1 base cuz humans start at 1
static int kthread_consumer(void *arg) {
    int consumer_id = consumer_thread_number;
    consumer_thread_number += 1;
    while(!kthread_should_stop()) {
	 if(down_interruptible(&mutex)) { // has to be a semaphore that's all I know
                break;
	 }

	if(down_interruptible(&empty) {
		break;
	}
        //calculating elapsed time should go here
        for (int i=0; i<buffSize; i++) {
            task_struct *task = buffer + (i*sizeof(struct task_struct)); // get the particular task, stored in the buffer
            int currentTime = ktime_get_ns();
            int startTime = task->start_time; // dereference and access start_time
            int telapsed = currentTime - startTime;
            printk("[Consumer-%d] Consumed Item#-%d on buffer index:%d PID:%d Elapsed Time - %s",
                consumer_id,
                0,
                i,
                123456,
                format_time(telapsed),
                0 // i dont know why but it made me add another argument to fix the error. Not sure why it's expecting 6 arguments in stead of just 5... maybe I'm being stupid.
            );
        }
	up(&mutex);
	up(&empty);
    }
}

static void exit_producer_consumer(void) {
    //exit the module
    printk("The total elapsed time of all processes for...")
}
