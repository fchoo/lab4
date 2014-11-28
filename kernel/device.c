/**
 * @file device.c
 *
 * Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * Date:    Tues, 22 Nov 2014 01:51:29 -0400
 */

#include <types.h>
#include <assert.h>
#include <globals.h>

#include <task.h>
#include <sched.h>
#include <device.h>
#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>

/**
 * @brief Fake device maintainence structure.
 * Since our tasks are periodic, we can represent
 * tasks with logical devices.
 * These logical devices should be signalled periodically
 * so that you can instantiate a new job every time period.
 * Devices are signaled by calling dev_update
 * on every timer interrupt. In dev_update check if it is
 * time to create a tasks new job. If so, make the task runnable.
 * There is a wait queue for every device which contains the tcbs of
 * all tasks waiting on the device event to occur.
 */

struct dev
{
	tcb_t* sleep_queue;
	unsigned long   next_match;
};
typedef struct dev dev_t;

/* devices will be periodically signaled at the following frequencies */
const unsigned long dev_freq[NUM_DEVICES] = {100, 200, 500, 50};
static dev_t devices[NUM_DEVICES];

void print_devq()
{
    int i;
    tcb_t *head;
    printf("\t[INFO] Printing device sleep queue...\n");
    for (i=0; i<NUM_DEVICES;i++) {
        printf("\t\tDevice %d: ", i);
        head = devices[i].sleep_queue;
        while (head != NULL) {
            printf("| %d ", head->cur_prio);
            head = head->sleep_queue;
        }
        printf("|\n");
    }
}

/**
 * @brief Initialize the sleep queues and match values for all devices.
 */
void dev_init(unsigned long millis)
{
    int i;
    // For each device initialize sleep queues and match values
    for (i=0; i<NUM_DEVICES; i++) {
        devices[i].sleep_queue = NULL;
        devices[i].next_match = millis + dev_freq[i];
    }
    dev_offset = millis;
}


/**
 * @brief Puts a task to sleep on the sleep queue until the next
 * event is signalled for the device.
 *
 * @param dev  Device number.
 */
void dev_wait(unsigned int dev)
{
    // printf("\t[INFO] Calling dev_wait...\n");
    // Invalid device number
    if (dev >= NUM_DEVICES) return;
    // Append current tcb to the end of device sleep queue
    if (devices[dev].sleep_queue != NULL) {
        tcb_t *dev_queue = devices[dev].sleep_queue;
        while (dev_queue->sleep_queue != NULL) {
            dev_queue = dev_queue->sleep_queue;
        }
        dev_queue->sleep_queue = get_cur_tcb();
    } else {
        // Insert into empty list
        devices[dev].sleep_queue = get_cur_tcb();
    }
}


/**
 * @brief Signals the occurrence of an event on all applicable devices.
 * This function should be called on timer interrupts to determine that
 * the interrupt corresponds to the event frequency of a device. If the
 * interrupt corresponded to the interrupt frequency of a device, this
 * function should ensure that the task is made ready to run
 */
void dev_update(unsigned long millis)
{
    // printf("\t[INFO] Calling dev_update...\n");
    int i;
    tcb_t *head;
    tcb_t *next;
    bool_e hasHigherPrio = FALSE;

    uint8_t curPrio = get_cur_prio();
    //print_devq();
    for (i=0; i<NUM_DEVICES; i++) {
        // if device is matched
        if (devices[i].next_match <= millis) {
            head = devices[i].sleep_queue;
            while (head != NULL) {
                // printf("\t\tCalling dev_update on dev(%d)\n", i);
                // Check if there is a higher priority task
                if (head->cur_prio < curPrio) hasHigherPrio = TRUE;
                // Add to run queue
                next = head->sleep_queue;
                runqueue_add(head, head->native_prio);
                printf("\t\tAdding task %d\n", head->native_prio);
                head = next;
            }
            // Clear sleep queue
            devices[i].sleep_queue = NULL;
            // Update next match for the device
            devices[i].next_match += dev_freq[i];
        }
    }
    // Context switch to higher priority
    if (hasHigherPrio) {
        dispatch_save();
    }
    // printf("\t[INFO] Exiting dev_update...\n");
}

