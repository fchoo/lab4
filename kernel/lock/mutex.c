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

}

int mutex_create(void)
{

	//iterate over the mutex array for an unused mutex (flag avail)
	//set mutex available to 0
	//return the index of this mutex
	//if max array size reached, return ENOMEM
	return 1; // fix this to return the correct value
}

int mutex_lock(int mutex  __attribute__((unused)))
{
	//if mutex index is not in array range or mutex not
	//available return EINVAL

	//if mutex is held by the curr task, return EDEADLOCK

	//check if mutex is being held by another task
	//if it is, check that task is of a lower priority
	//if task holding mutex is of lower priority
	//elevate said task to current task's priority
	//move current task to mutex sleep queue
	//call dispatch save

	//otherwise, give lock to current task
	//set tcb has lock to 1
	//set mutex bLock to 1
	//change holder of mutex to curr_tcb

	return 0; // return on success
}

int mutex_unlock(int mutex  __attribute__((unused)))
{

	//if mutex index is not in array range return EINVAL

	//if mutex is not held by the curr task, return EPERM

	//check if mutex sleep queue head is not NULL
	//if not null, set curr task priority to original priority
	//set curr_tcb hasLock to 0
	//change holder of mutex to target task
	//move waiting task from sleep queue to run queue (remember to set to null)
	//call dispatch save

	//otherwise, set holder of mutex to null
	//set mutex bLock to 0
	//set curr_tcb hasLock to 0

	return 1; // fix this to return the correct value
}

