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
#include <lock.h>

/* C_SWI_Handler uses SWI number to call the appropriate function. */
ssize_t C_SWI_Handler(int swiNum, int *regs) {
    ssize_t retVal = 0;
    switch (swiNum) {
        // ssize_t read(int fd, void *buf, size_t count);
        case READ_SWI:
            retVal = read_syscall(regs[0], (void *) regs[1], (size_t) regs[2]);
            break;
        // ssize_t write(int fd, const void *buf, size_t count);
        case WRITE_SWI:
            retVal = write_syscall((int) regs[0], (void *) regs[1], (size_t) regs[2]);
            break;
        // void sleep(unsigned long millis)
        case SLEEP_SWI:
            sleep_syscall((unsigned long) regs[0]);
            break;
	    // unsigned long time()
        case TIME_SWI:
            retVal = (ssize_t)time_syscall();
            break;
        // int task_create(task_t* tasks, size_t num_tasks)
        case CREATE_SWI:
            retVal = task_create((task_t*)regs[0], (size_t)regs[1]);
            break;
        // int event_wait(unsigned int dev_num)
        case EVENT_WAIT:
            retVal = event_wait((unsigned int)regs[0]);
            break;
        // int mutex_create(void);
        case MUTEX_CREATE:
            retVal = mutex_create();
            break;
        // int mutex_lock(int mutex);
        case MUTEX_LOCK:
            retVal = mutex_lock(regs[0]);
            break;
        // int mutex_unlock(int mutex);
        case MUTEX_UNLOCK:
            retVal = mutex_unlock(regs[0]);
            break;
        // Invalid swi num
        default:
	    invalid_syscall(swiNum);
    }

    return retVal;
}

