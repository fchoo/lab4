/** @file time.S
 *
 * @brief time sycall wrapper
 *
 * @authors Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * @date    11/22/2014
 */

#include <asm.h>
#include <bits/swi.h>

	.file "task_create.S"

FUNC(task_create)
    swi CREATE_SWI @make the system call

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
