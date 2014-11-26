/** @file lock.h
 *
 * @brief Declaration of locking and synchronization primitives.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-12-01
 *
 * Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * Date:    Tues, 22 Nov 2014 01:51:29 -0400
 */

#include <types.h>
#include <task.h>

#ifndef _LOCK_H_
#define _LOCK_H_

struct mutex
{
	bool_e	bAvailable;		/* flag for availability */
	tcb_t*	pHolding_Tcb;	/* who are using this mutex */
	bool_e	bLock;			/* 1 for lock/0 for unlock */
	tcb_t*	pSleep_queue;	/* list of applications waiting for this mutex */
};
typedef struct mutex mutex_t;

struct cond
{
	/* FILL THIS IN */
	/* is not handled in this lab */
};
typedef struct cond cond_t;

void mutex_init(void);	/* a function for initiating mutexes */
int mutex_create(void);
int mutex_lock(int mutex);
int mutex_unlock(int mutex);

#endif /* _LOCK_H_ */
