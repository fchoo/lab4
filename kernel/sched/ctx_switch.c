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
	printf("[INFO] Calling dispatch_save\n");
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
	printf("[INFO] Calling dispatch_nosave!\n");

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
	printf("sleep!\n");
	//1) get curr list of runnables
	//2) if list not empty, context switch to next highest prior task
	//3) if list empty, context switch to the idle task

    //disable interrupts
    disable_interrupts();

    //get the tcb of task (63 is the idle task)
    tcb_t* next_tcb = runqueue_remove(highest_prio());

    //get old and new contexts
    sched_context_t  contxt_old = cur_tcb->context;

    printf("lr %x sp %x r4 %x r5 %x r6 %x r7 %x r8 %x r9 %x r10 %x\n", contxt_old.lr, contxt_old.sp, contxt_old.r4, contxt_old.r5, 
			contxt_old.r6, contxt_old.r7, contxt_old.r8, contxt_old.r9, 
			contxt_old.r10);
    cur_tcb = next_tcb;
    sched_context_t  contxt_new = cur_tcb->context;

    printf("lr %x sp %x r4 %x r5 %x r6 %x r7 %x r8 %x r9 %x r10 %x\n", contxt_new.lr, contxt_new.sp, contxt_new.r4, contxt_new.r5,
			contxt_new.r6, contxt_new.r7, contxt_new.r8, contxt_new.r9,
			contxt_new.r10);


    //context switch full
    ctx_switch_full(&contxt_new, &contxt_old);


    //renable interupts
    enable_interrupts();

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
