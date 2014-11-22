/** @file sched.c
 *
 * @brief Top level implementation of the scheduler.
 *
 * Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * Date:    Tues, 11 Nov 2014 01:51:29 -0400
 */

#include <types.h>
#include <assert.h>

#include <kernel.h>
#include <config.h>
#include "sched_i.h"

#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/physmem.h>

tcb_t system_tcb[OS_MAX_TASKS]; /*allocate memory for system TCBs */

void sched_init(task_t* main_task  __attribute__((unused)))
{
}

/**
 * @brief This is the idle task that the system runs when no other task is runnable
 */

static void idle(void)
{
	 enable_interrupts();
	 while(1);
}

/**
 * @brief Allocate user-stacks and initializes the kernel contexts of the
 * given threads.
 *
 * This function assumes that:
 * - num_tasks < number of tasks allowed on the system.
 * - the tasks have already been deemed schedulable and have been appropriately
 *   scheduled.  In particular, this means that the task list is sorted in order
 *   of priority -- higher priority tasks come first.
 *
 * @param tasks  A list of scheduled task descriptors.
 * @param size   The number of tasks is the list.
 */
void allocate_tasks(task_t** tasks , size_t num_tasks)
{
    // Initialize run queue
    runqueue_init();

    // Initialize idle tcb
    system_tcb[IDLE_PRIO].native_prio = IDLE_PRIO;
    system_tcb[IDLE_PRIO].cur_prio = IDLE_PRIO;
    system_tcb[IDLE_PRIO].holds_lock = 0;
    system_tcb[IDLE_PRIO].sleep_queue = NULL;

    system_tcb[IDLE_PRIO].context.r4 = (uint32_t)idle;
    system_tcb[IDLE_PRIO].context.r5 = 0;
    system_tcb[IDLE_PRIO].context.r6 = 0;
    system_tcb[IDLE_PRIO].context.sp = system_tcb[IDLE_PRIO].kstack_high;
    system_tcb[IDLE_PRIO].context.lr = 0;

    dispatch_init(&system_tcb[IDLE_PRIO]);

    // Erroneous amount of num task
    if (num_tasks > OS_AVAIL_TASKS) return;

    size_t prio = 0;
    task_t *curTask;
    // Assumes that the task list is sorted by priority previously by ub test
    for (prio=0; prio<num_tasks; prio++) {
        curTask = tasks[prio];
        // check for erroneous tasks
        // Note: AAPCS wants 8 byte alignment
        if (!is_userSpace((uint32_t)curTask->lambda)  ||
            !is_userSpace((uint32_t)curTask->data)       ||
            !is_stackAligned((uint32_t)curTask->stack_pos)) {
            return;
        }

        // Initialize tcb for task
        system_tcb[prio].native_prio = prio;
        system_tcb[prio].cur_prio = prio;
        system_tcb[prio].holds_lock = 0;
        system_tcb[prio].sleep_queue = NULL;
        // Set up context for launch_task
        system_tcb[prio].context.r4 = (uint32_t)curTask->lambda;
        system_tcb[prio].context.r5 = (uint32_t)curTask->data;
        system_tcb[prio].context.r6 = (uint32_t)curTask->stack_pos;
        system_tcb[prio].context.sp = system_tcb[prio].kstack_high;
        system_tcb[prio].context.lr = 0;
        // Add task to run queue
        runqueue_add(&system_tcb[prio], (uint8_t)prio);
    }

}

