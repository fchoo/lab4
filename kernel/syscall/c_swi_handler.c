/*
 * kernel.c: Kernel main (entry) function
 *
 * Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * Date:    Tues, 11 Nov 2014 01:51:29 -0400
 */

#include <exports.h>
#include <bits/errno.h>
#include <bits/fileno.h>
#include <bits/swi.h>

#include <globals.h>
#include <syscall.h>

/* C_SWI_Handler uses SWI number to call the appropriate function. */
ssize_t C_SWI_Handler(int swiNum, int *regs) {
    ssize_t count = 0;
    switch (swiNum) {
        // ssize_t read(int fd, void *buf, size_t count);
        case READ_SWI:
            count = read_syscall(regs[0], (void *) regs[1], (size_t) regs[2]);
            break;
        // ssize_t write(int fd, const void *buf, size_t count);
        case WRITE_SWI:
            count = write_syscall((int) regs[0], (void *) regs[1], (size_t) regs[2]);
            break;
        // void sleep(unsigned long millis)
        case SLEEP_SWI:
            sleep_syscall((unsigned long) regs[0]);
            break;
	    // unsigned long time()
        case TIME_SWI:
            count = (ssize_t)time_syscall();
            break;
        // int task_create(task_t* tasks, size_t num_tasks)
        case CREATE_SWI:
            count = task_create((task_t*)regs[0], (size_t)regs[1]);
            break;
        // int event_wait(unsigned int dev_num)
        case EVENT_WAIT:
            count = event_wait((unsigned int)regs[0]);
            break;
        case MUTEX_CREATE:
            break;
        case MUTEX_LOCK:
            break;
        case MUTEX_UNLOCK:
            break;

        default:
	    invalid_syscall(swiNum);
    }

    return count;
}

