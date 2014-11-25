/** @file ctx_switch.c
 *
 * @brief C wrappers around assembly context switch routines.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-21
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
	printf("\n[INFO] Calling dispatch_save\n");
    // get the tcb of task (63 is the idle task)
    tcb_t *next_tcb = runqueue_remove(highest_prio());
    // add curr tcb add back to runnables
    runqueue_add(cur_tcb, get_cur_prio());

    // Update cur_tcb
    tcb_t *old_tcb = cur_tcb;
    cur_tcb = next_tcb;

    //context switch full
    ctx_switch_full(&(cur_tcb->context), &(old_tcb->context));

}

/**
 * @brief Context switch to the highest priority task that is not this task --
 * don't save the current task state.
 *
 * There is always an idle task to switch to.
 */
void dispatch_nosave(void)
{
	printf("\n[INFO] Calling dispatch_nosave!\n");

    //get the tcb of task (63 is the idle task)
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
	printf("\n[INFO] Calling dispatch_sleep!\n");

    //get the tcb of task (63 is the idle task)
    tcb_t* next_tcb = runqueue_remove(highest_prio());

    // Update cur_tcb
    tcb_t *old_tcb = cur_tcb;
    cur_tcb = next_tcb;

    printf("\n[INFO] lr(%x) sp(%x) r4(%x) r5(%x) r6(%x) r7(%x) r8(%x) r9(%x) r10(%x)\n",
           old_tcb->context.lr, old_tcb->context.sp, old_tcb->context.r4, old_tcb->context.r5,
			old_tcb->context.r6, old_tcb->context.r7, old_tcb->context.r8, old_tcb->context.r9,
			old_tcb->context.r10);

    printf("\n[INFO] lr(%x) sp(%x) r4(%x) r5(%x) r6(%x) r7(%x) r8(%x) r9(%x) r10(%x)\n",
           cur_tcb->context.lr, cur_tcb->context.sp, cur_tcb->context.r4, cur_tcb->context.r5,
			cur_tcb->context.r6, cur_tcb->context.r7, cur_tcb->context.r8, cur_tcb->context.r9,
			cur_tcb->context.r10);


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
