/** @file crt0.S
 *
 * @brief Execution startup routine
 *
 * @authors Foo Lai Choo <wkuan@andrew.cmu.edu>
 *          Hui Jun Tay <chinyano@andrew.cmu.edu>
 * @date    11/16/2014
 */


#include <asm.h>

    .file "crt0.S"

FUNC(_start)
    @ Load argv and argc into arg registers
    ldr     r0, [sp]    @ Load argc
    add     r1, sp, #4  @ Load argv**

    @ Call main
    mov     lr, pc
    ldr     pc, =main

    @ Infinite loop after main returns.
    sub     pc, pc, #8;
