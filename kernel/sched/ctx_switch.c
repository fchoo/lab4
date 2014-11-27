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
    // printf("\t[INFO] Calling dispatch_save...\n");
    // get the tcb of task (63 is the idle task)
    tcb_t *next_tcb = runqueue_remove(highest_prio());
    // add curr tcb add back to runnables
    runqueue_add(cur_tcb, get_cur_prio());

    // Update cur_tcb
    tcb_t *old_tcb = cur_tcb;
    cur_tcb = next_tcb;

    // printf("\t\t[INFO] Switch and execute task with prio(%d), lr(%x)\n",
    //            cur_tcb->cur_prio, cur_tcb->context.lr);
    //context switch full
    ctx_switch_full(&(cur_tcb->context), &(old_tcb->context));
    // Renable interrupts
    //enable_interrupts();
}

/**
 * @brief Context switch to the highest priority task that is not this task --
 * don't save the current task state.
 *
 * There is always an idle task to switch to.
 */
void dispatch_nosave(void)
{
    //get the tcb of task (63 is the idle task)
    //get the tcb of task
    tcb_t* next_tcb = runqueue_remove(highest_prio());
    cur_tcb = next_tcb;

    ctx_switch_half(&(cur_tcb->context));
}


/**
 * @brief Context switch to the highest priority task that is not this task --
 * and save the current task but don't mark is runnable.
 *
 * There is always an idle task to switch to.
 */
void dispatch_sleep(void)
{
    // printf("\t[INFO] Calling dispatch_sleep...\n");
    //get the tcb of task (63 is the idle task)
    tcb_t* next_tcb = runqueue_remove(highest_prio());

    // Update cur_tcb
    tcb_t *old_tcb = cur_tcb;
    cur_tcb = next_tcb;


    //context switch half since we are never returning from dispatch_sleep!
    // printf("\t\t[INFO] Switch and execute task with prio(%d), lr(%x)\n",
           // cur_tcb->cur_prio, cur_tcb->context.lr);
    //context switch full
    ctx_switch_full(&(cur_tcb->context), &(old_tcb->context));
}

void dispatch_mlock(tcb_t* target_tcb)
{
    //remove curr task from run queue       
    runqueue_remove(get_cur_prio());
    
    //remove target task from run queue       
    runqueue_remove(target_tcb->native_prio);

    //set curr task to holding task
    tcb_t *old_tcb = cur_tcb;
    cur_tcb = target_tcb;

    //get context of lock-holding task and curr task
    //context switch full
    ctx_switch_full(&(cur_tcb->context), &(old_tcb->context));
}

void dispatch_munlock(tcb_t* target_tcb)
{
    //add curr task to run queue       
    runqueue_add(cur_tcb, get_cur_prio());
    
    //set curr task to target task
    tcb_t *old_tcb = cur_tcb;
    cur_tcb = target_tcb;

    //get context of lock-holding task and curr task
    //context switch full
    ctx_switch_full(&(cur_tcb->context), &(old_tcb->context));
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

