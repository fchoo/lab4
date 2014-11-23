/** @file ub_test.c
 *
 * @brief The UB Test for basic schedulability
 *
 * Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * Date:    Tues, 22 Nov 2014 01:51:29 -0400
 */

//#define DEBUG 0

#include <sched.h>
#ifdef DEBUG
#include <exports.h>
#endif

#include <types.h>

/**
 * @brief Perform UB Test and reorder the task list.
 *
 * The task list at the end of this method will be sorted in order is priority
 * -- from highest priority (lowest priority number) to lowest priority
 * (highest priority number).
 *
 * @param tasks  An array of task pointers containing the task set to schedule.
 * @param num_tasks  The number of tasks in the array.
 *
 * @return 0  The test failed.
 * @return 1  Test succeeded.  The tasks are now in order.
 */
int assign_schedule(task_t** tasks, size_t num_tasks)
{
    if (tasks == NULL || num_tasks == 0) return 0; // remove once implemented
	return 1; // fix this; dummy return to prevent compiler warnings
}



