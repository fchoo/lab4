/** @file proc.c
 *
 * @brief Implementation of `process' syscalls
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Sun, 14 Oct 2007 00:07:38 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-12
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

int task_create(task_t* tasks  __attribute__((unused)), size_t num_tasks  __attribute__((unused)))
{
    // TODO: task_create
    // 1) Create a task_t array sorted_tasks based on num_tasks and tasks;
    // 2) sched_init, runqueue_int, mutex_init
    // 3) maybe dev_init?
    // NOTE: DISABLE IRQ?
    // 2) Call assign_schedule(sorted_tasks, num_tasks) -- sched/ub_test.c
    // 3) Call allocate_tasks(sorted_tasks, num_tasks) -- sched/sched.c
    // 4) Start device interrupt timer
    // 5) Call dispatch_nosave() -- sched/ctx_switch.c
    // NOTE: ENABLE IRQ?

    return 1; /* remove this line after adding your code */
}

int event_wait(unsigned int dev  __attribute__((unused)))
{
    //TODO: event_wait
    //1) call dev_wait with device no. as parameter
    //should never return
    return 1; /* remove this line after adding your code */
}

/* An invalid syscall causes the kernel to exit. */
void invalid_syscall(unsigned int call_num  __attribute__((unused)))
{
	printf("Kernel panic: invalid syscall -- 0x%08x\n", call_num);

	disable_interrupts();
	while(1);
}
