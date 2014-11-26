/** @file mutex_unlock.S
 *
 * @brief mutex unlock sycall wrapper
 *
 * Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * Date:    Tues, 21 Nov 2014 01:51:29 -0400
 */

#include <asm.h>
#include <bits/swi.h>

	.file "mutex_unlock.S"

FUNC(mutex_unlock)
    swi MUTEX_UNLOCK @make the system call

    @ Return on success
    cmp r0, #0 @success
    movpl pc, lr @pl means >= 0

    @ Store the absolute value of return value in errno
    neg r0, r0
    ldr r1, =errno
    str r0, [r1]

    @ Store -1 in return value
    mov r0, #-1
    mov pc, lr
