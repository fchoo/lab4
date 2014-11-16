/** @file time.S
 *
 * @brief time sycall wrapper
 *
 * Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * Date:    Tues, 11 Nov 2014 01:51:29 -0400
 */

#include <asm.h>
#include <bits/swi.h>

    .file "time.S"

FUNC(time)
    @ r0 contains the time from c_swi_handler
    swi TIME_SWI @ make the system call
    mov pc, lr
