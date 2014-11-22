@ swi_handler.S: SWI Handler
@
@ Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
@          Hui Jun Tay <htay@andrew.cmu.edu>
@ Date:    Tues, 11 Nov 2014 01:51:29 -0400

    .extern C_SWI_Handler

    .file   "swi_handler.S"
    .text

    .global swi_handler
swi_handler:
    sub     sp, sp, #4
    stmfd   sp!, {r0-r12, lr}

    @ get spsr into general purpose (gp) registers
    mrs     r2, spsr

    @ store spsr above gp registers
    str     r2, [sp, #14*4]

    @mask the cpsr so irqs are enabled
    msr     cpsr_c, #(PSR_MODE_SVC | PSR_FIQ)
    @mrs     r2, cpsr
    @bic     r2, r2, #0x80
    @msr     cpsr, r2

    @ Restore jmp table in r8
    ldr ip, =global_data
    ldr r8, [ip]

    @ Set Param 1: extract SWI and get SWI no. by bit-masking
    ldr     r0, [lr, #-4]
    bic     r0, r0, #0xff000000

    @ Set Param 2: pointer to parameters on stack
    mov     r1, sp

    @ go to handler
    bl      C_SWI_Handler

    @ restore spsr
    ldr     r2, [sp, #14*4]
    msr     spsr, r2

    @ Discard the r0 on the stack (r0 has ret value).
    add     sp, sp, #4
    ldmfd   sp!, {r1-r12, lr}

    @ remove space used to store spsr
    add     sp, sp, #4
    movs    pc, lr
