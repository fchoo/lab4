/** @file proc.c
 *
 * @brief Implementation of `process' syscalls
 *
 * Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * Date:    Tues, 22 Nov 2014 01:51:29 -0400
 */

#include <exports.h>
#include <bits/errno.h>
#include <config.h>
#include <kernel.h>
#include <syscall.h>
#include <sched.h>

#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/physmem.h>
#include <device.h>

int task_create(task_t* tasks, size_t num_tasks)
{
    // Unable to schedule the number of tasks
    if (num_tasks > OS_AVAIL_TASKS) return -EINVAL;

    task_t* sorted_tasks[num_tasks];
    size_t i ;
    for (i=0; i<num_tasks; i++) {
        sorted_tasks[i] = tasks + i; // Increment pointer and put into array
    }

    disable_interrupts(); // Disable interrupt when modifying shared structs
    // UB Test
    if (!assign_schedule(sorted_tasks, num_tasks)) return -ESCHED;

    // Create tasks in kernel
    allocate_tasks(sorted_tasks, num_tasks);

    // TODO: Start device interrupt timer

    // Start the highest priority task
    dispatch_nosave();
    // Re-enable interrupts at the end
    enable_interrupts();
    return 0;
}

int event_wait(unsigned int dev)
{
    // Invalid device number
    if (dev >= NUM_DEVICES) return -EINVAL;
    // Disable interrupt to prevent changes to cur_tcb
    disable_interrupts();
    // Get cur_tcb
    tcb_t* cur_tcb = get_cur_tcb();
    // Prevent tasks from waiting if they have a mutex.
    if (cur_tcb->holds_lock != 0) return -EHOLDSLOCK;
    // Restore device priority
    cur_tcb->cur_prio = cur_tcb->native_prio;
    // Put calling task back to device queue
    dev_wait(dev);
    // Switch to next highest priority task
    dispatch_sleep();
    // Re-enable interrupt at the end
    enable_interrupts();
    return 0;
}

/* An invalid syscall causes the kernel to exit. */
void invalid_syscall(unsigned int call_num  __attribute__((unused)))
{
	printf("Kernel panic: invalid syscall -- 0x%08x\n", call_num);

	disable_interrupts();
	while(1);
}
