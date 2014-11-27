/**
 * @file mutex.c
 *
 * @brief Implements mutices.
 *
 * Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * Date:    Tues, 22 Nov 2014 01:51:29 -0400
 */

//#define DEBUG_MUTEX

#include <lock.h>
#include <task.h>
#include <sched.h>
#include <bits/errno.h>
#include <arm/psr.h>
#include <arm/exception.h>
#ifdef DEBUG_MUTEX
#include <exports.h> // temp
#endif

mutex_t gtMutex[OS_NUM_MUTEX];

void mutex_init()
{
	int i;
	for (i=0; i<OS_NUM_MUTEX; i++) {
		gtMutex[i].bAvailable = TRUE;
		gtMutex[i].pHolding_Tcb = NULL;
		gtMutex[i].bLock = FALSE;
		gtMutex[i].pSleep_queue = NULL;
	}
}

int mutex_create(void)
{
	int i;
	for (i=0; i<OS_NUM_MUTEX; i++) {
		// Skip if mutex has been allocated
		if (gtMutex[i].bAvailable == FALSE) continue;
		// Else de-avail mutex and return index
		gtMutex[i].bAvailable = FALSE;
		return i;
	}
	// Did not manage to find an available mutex
	return -ENOMEM;
}

int mutex_lock(int mutex  __attribute__((unused)))
{
	//if mutex index is not in array range or mutex not
	//available return EINVAL
	if(mutex < 0 || mutex >= 32 || gtMutex[mutex].bAvailable) {
		return EINVAL;
	}

	mutex_t mtx = gtMutex[mutex];
	tcb_t* curr_tcb = get_cur_tcb();
	//if mutex is held by the curr task, return EDEADLOCK
	if(mtx.pHolding_Tcb == curr_tcb) {
		return EDEADLOCK;
	}

	//check if mutex is being held by another task
	if(mtx.bLock == TRUE) {
		//if it is, check that tk is of a lower priority
		tcb_t* hold_tcb = mtx.pHolding_Tcb;

		//if task holding mutex is of lower priority
		if(hold_tcb->cur_prio < curr_tcb->cur_prio) {
			//elevate holding task to cur_task prio
			hold_tcb->cur_prio = curr_tcb->cur_prio;

			//move current task to mutex sleep queue
			tcb_t* head = mtx.pSleep_queue;
			if(head == NULL) {
				mtx.pSleep_queue = curr_tcb;
			}
			else {
				while(head->sleep_queue != NULL) {
					head = head->sleep_queue;
				}
				head->sleep_queue = curr_tcb;
			}

			//dispatch_mlock
			dispatch_mlock(hold_tcb);

		}
		else {
			//should never get here
		}
	}
	else {
		//otherwise, give lock to current task
		mtx.pHolding_Tcb = curr_tcb;
		//set tcb has lock to 1
		curr_tcb->holds_lock = 1;
		//set mutex bLock to 1
		mtx.bLock = TRUE;
	}

	return 0; // return on success
}

int mutex_unlock(int mutex  __attribute__((unused)))
{
        //if mutex index is not in array range return EINVAL
        if(mutex < 0 || mutex >= 32 || gtMutex[mutex].bAvailable) {
                return EINVAL;
        }

        mutex_t mtx = gtMutex[mutex];
        //if mutex is not held by the curr task, return EPERM
        tcb_t* curr_tcb = get_cur_tcb();
        if(mtx.pHolding_Tcb != curr_tcb) {
                return EPERM;
        }

	//check if mutex sleep queue head is not NULL
	if(mtx.pSleep_queue != NULL) {
		//if not null, set curr task priority to original priority
		curr_tcb->cur_prio = curr_tcb->native_prio;

		//set curr_tcb hasLock to 0
		curr_tcb->holds_lock = 0;

		//move waiting task from sleep queue to run queue (remember to set to null)
		tcb_t* target_tcb = mtx.pSleep_queue;
		if(target_tcb->sleep_queue == NULL) {
			mtx.pSleep_queue = NULL;
		}
		else {
			tcb_t* old_tcb = target_tcb;
			while(target_tcb->sleep_queue != NULL) {
				old_tcb = target_tcb;
				target_tcb = target_tcb->sleep_queue;
			}
			old_tcb->sleep_queue = NULL;
		}

		//change holder of mutex to target task
		mtx.pHolding_Tcb = target_tcb;

		//call dispatch munlock
		dispatch_munlock(target_tcb);
	}
	else {
		//otherwise, set holder of mutex to null
		mtx.pHolding_Tcb = NULL;
		//set mutex bLock to 0
		mtx.bLock = 0;
		//set curr_tcb hasLock to 0
		curr_tcb->holds_lock = 0;
	}

	return 1; // fix this to return the correct value
}

