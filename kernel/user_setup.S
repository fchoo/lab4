@ user_setup.S: helper function written in asm called from kernel
@
@ Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
@          Hui Jun Tay <htay@andrew.cmu.edu>
@ Date:    Tues, 11 Nov 2014 01:51:29 -0400


#include <arm/psr.h>
#include <config.h>

    .file   "user_setup.S"
    .text

    .global user_setup
user_setup:
    @ Note we are not returning to kernel anymore hence we do not have to save
    @ the registers, lr and cpsr

    @ change mode to USER, disable FIQ
    @ Address only the last 2 bytes of cpsr
    msr     cpsr_c, #(PSR_MODE_USR | PSR_FIQ)

    @ set up user stack from spTop passed in from kernel.c
    mov     sp, r0

    @ go to user program
    ldr     pc, =LOAD_ADDR

    @ Reload user program as backup. It should not reach here...
    sub     pc, pc, #8
