/** @file sleep.S
 *
 * @brief sleep sycall wrapper
 *
 * Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * Date:    Tues, 11 Nov 2014 01:51:29 -0400
 */

#include <asm.h>
#include <bits/swi.h>

    .file "sleep.S"

FUNC(sleep)
    swi SLEEP_SWI @make the system call
    mov pc, lr
