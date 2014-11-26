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

/* This UB rate table stores all the utilization rate until the 64th tasks.
 * Technically, we only need only until the 63th tasks since OS_AVAIL_TASKS is 63.
 */
static double ub_rate_table[] =
{
1.00000, 0.82843, 0.77976, 0.75683, 0.74349, 0.73477, 0.72863, 0.72406,
0.72054, 0.71773, 0.71545, 0.71356, 0.71196, 0.71059, 0.70941, 0.70838,
0.70747, 0.70667, 0.70595, 0.70530, 0.70471, 0.70418, 0.70370, 0.70325,
0.70285, 0.70247, 0.70212, 0.70180, 0.70150, 0.70122, 0.70095, 0.70071,
0.70048, 0.70026, 0.70006, 0.69986, 0.69968, 0.69951, 0.69934, 0.69919,
0.69904, 0.69890, 0.69876, 0.69864, 0.69851, 0.69840, 0.69828, 0.69818,
0.69807, 0.69797, 0.69788, 0.69779, 0.69770, 0.69761, 0.69753, 0.69745,
0.69738, 0.69731, 0.69723, 0.69717, 0.69710, 0.69704, 0.69697, 0.69691
};

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
    // Check for invalid num_tasks
    if (num_tasks > OS_AVAIL_TASKS) return 0;

    // Selection sort the tasks
    uint32_t i, j, highest_prio;
    for (i = 0; i < num_tasks; i++) {
        highest_prio = i;
        for (j = i + 1; j < num_tasks; j++) {
            // Compare tasks' periods
            if (tasks[j]->T < tasks[highest_prio]->T) {
                highest_prio = j;
            }
        }
        // Swap in place.
        task_t* task_highest_prio = tasks[highest_prio];
        tasks[highest_prio] = tasks[i];
        tasks[i] = task_highest_prio;
    }

    // Perform UB test
    for (i = 0; i < num_tasks; i++) {
        double curr_u = 0.0;
        // Check for division by zero error
        if (tasks[i]->T == 0) return 0;
        // Sum all utilizations up till current task
        for (j = 0; j <= i; j++) {
            curr_u += (tasks[j]->C / tasks[j]->T);
        }
        // Add in blocking term
        curr_u += (tasks[i]->B / tasks[i]->T);
        // Check ub rate for every task
        if (curr_u >= ub_rate_table[i]) return 0;
    }
    return 1;
}



