/** @file ctx_switch.c
 *
 * @brief C wrappers around assembly context switch routines.
 *
 * Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * Date:    Tues, 21 Nov 2014 01:51:29 -0400
 */


#include <types.h>
#include <assert.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <config.h>
#include <kernel.h>
#include "sched_i.h"

#ifdef DEBUG_MUTEX
#include <exports.h>
#endif

static tcb_t* cur_tcb; /* use this if needed */

/**
 * @brief Initialize the current TCB and priority.
 *
 * Set the initialization thread's priority to IDLE so that anything
 * will preempt it when dispatching the first task.
 */
void dispatch_init(tcb_t* idle)
{
    cur_tcb = idle;
    runqueue_add(cur_tcb, cur_tcb->cur_prio);
}


/**
 * @brief Context switch to the highest priority task while saving off the
 * current task state.
 *
 * This function needs to be externally synchronized.
 * We could be switching from the idle task.  The priority searcher has been tuned
 * to return IDLE_PRIO for a completely empty run_queue case.
 */
void dispatch_save(void)
{
    printf("[INFO] Calling dispatch save\n");

    printf("highest prio %d\n", highest_prio());
    // get the tcb of task (63 is the idle task)
    tcb_t *next_tcb = runqueue_remove(highest_prio());
    // add curr tcb add back to runnables
    runqueue_add(cur_tcb, get_cur_prio());

    // Update cur_tcb
    tcb_t *old_tcb = cur_tcb;
    cur_tcb = next_tcb;

    printf("[INFO] oldtcb %d newtch %d highest_prio %d\n", old_tcb->cur_prio , cur_tcb->cur_prio, highest_prio());
    //context switch full
    ctx_switch_full(&(cur_tcb->context), &(old_tcb->context));
    // Renable interrupts
    //enable_interrupts();
    printf("[INFO] Leaving dispatch save\n");
}

/**
 * @brief Context switch to the highest priority task that is not this task --
 * don't save the current task state.
 *
 * There is always an idle task to switch to.
 */
void dispatch_nosave(void)
{
    printf("[INFO] Calling dispatch nosave\n");
    //get the tcb of task (63 is the idle task)
    tcb_t* next_tcb = runqueue_remove(highest_prio());
    cur_tcb = next_tcb;

    ctx_switch_half(&(cur_tcb->context));
    printf("[INFO] Leaving dispatch nosave\n");
}


/**
 * @brief Context switch to the highest priority task that is not this task --
 * and save the current task but don't mark is runnable.
 *
 * There is always an idle task to switch to.
 */
void dispatch_sleep(void)
{
    printf("[INFO] Calling dispatch sleep\n");
    printf("highest prio %d\n", highest_prio());
    //get the tcb of task (63 is the idle task)
    tcb_t* next_tcb = runqueue_remove(highest_prio());

    printf("[INFO] oldtcb %d newtch %d ", cur_tcb->cur_prio , next_tcb->cur_prio);
    printf("highest prio %d\n", highest_prio());
    // Update cur_tcb
    cur_tcb = next_tcb;


    //context switch half since we are never returning from dispatch_sleep!
    ctx_switch_half(&(cur_tcb->context));
    printf("[INFO] Leaving dispatch sleep\n");
}

/**
 * @brief Returns the priority value of the current task.
 */
uint8_t get_cur_prio(void)
{
	return cur_tcb->cur_prio;
}

/**
 * @brief Returns the TCB of the current task.
 */
tcb_t* get_cur_tcb(void)
{
	return cur_tcb;
}
