/** @file run_queue.c
 *
 * @brief Run queue maintainence routines.
 *
 * Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * Date:    Tues, 21 Nov 2014 01:51:29 -0400
 */

#include <types.h>
#include <assert.h>

#include <kernel.h>
#include <sched.h>
#include "sched_i.h"



static tcb_t* run_list[OS_MAX_TASKS];

/* A high bit in this bitmap means that the task whose priority is
 * equal to the bit number of the high bit is runnable.
 */
static uint8_t run_bits[OS_MAX_TASKS/8];

/* This is a trie structure.  Tasks are grouped in groups of 8.  If any task
 * in a particular group is runnable, the corresponding group flag is set.
 * Since we can only have 64 possible tasks, a single byte can represent the
 * run bits of all 8 groups.
 */
static uint8_t group_run_bits;

/* This unmap table finds the bit position of the lowest bit in a given byte
 * Useful for doing reverse lookup.
 */
static uint8_t prio_unmap_table[] =
{

0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};

/**
 * @brief Clears the run-queues and sets them all to empty.
 */
void runqueue_init(void)
{
    int i;
    // initialize run bits
    for (i = 0; i < OS_MAX_TASKS / 8; i++) {
        run_bits[i] = 0;
    }
    // initialize group run bits
    group_run_bits = 0;
    // initialize run list
    for (i = 0; i < OS_MAX_TASKS; i++) {
        run_list[i] = NULL;
    }
}

/**
 * @brief Adds the thread identified by the given TCB to the runqueue at
 * a given priority.
 *
 * The native priority of the thread need not be the specified priority.  The
 * only requirement is that the run queue for that priority is empty.  This
 * function needs to be externally synchronized.
 */
void runqueue_add(tcb_t* tcb, uint8_t prio)
{
    // Error prio input
    if (prio > OS_MAX_TASKS - 1) return;

    // Add task to run list. Append to end of priority run list if queue exist.
    if (run_list[prio] != NULL) {
        // Find the end of list
        tcb_t *prioRunList = run_list[prio];
        while (prioRunList->sleep_queue != NULL)
            prioRunList = prioRunList->sleep_queue;
        // Append to the end of list
        prioRunList->sleep_queue = tcb;
    } else {
        // Insert into empty list
        run_list[prio] = tcb;
    }

    // Set priority group and position
    uint8_t OSTCBY = prio >> 3;
    uint8_t OSTCBX = prio & 0x07;
    group_run_bits |= (1 << OSTCBY);
    run_bits[OSTCBY] |= (1 << OSTCBX);
}


/**
 * @brief Empty the run queue of the given priority.
 *
 * @return  The tcb at enqueued at the given priority.
 *
 * This function needs to be externally synchronized.
 */
tcb_t* runqueue_remove(uint8_t prio)
{
    // Priority does not match task num
    if (prio > OS_MAX_TASKS - 1) return NULL;
    // Requested when there is no task of that priority waiting
    else if (run_list[prio] == NULL) return NULL;

    tcb_t *runnable = run_list[prio];
    // Remove runnable task from run list
    if (runnable->sleep_queue != NULL) {
        // If runnable has more task after it, set it in priority run list
        run_list[prio] = runnable->sleep_queue;
        runnable->sleep_queue = NULL;
    } else {
        // Else, empty priority run list
        run_list[prio] = NULL;
    }

    // Clear priority group and position associated with that priority.
    uint8_t OSTCBY = prio >> 3;
    uint8_t OSTCBX = prio & 0x07;
    run_bits[OSTCBY] &= ~(1 << OSTCBX);
    if (run_bits[OSTCBY] == 0) group_run_bits &= ~(1 << OSTCBY);

    return runnable;
}

/**
 * @brief This function examines the run bits and the run queue and returns the
 * priority of the runnable task with the highest priority (lower number).
 */
uint8_t highest_prio(void)
{
    uint8_t y = prio_unmap_table[group_run_bits];
    uint8_t x = prio_unmap_table[run_bits[y]];
    return (y << 3) + x;
}
