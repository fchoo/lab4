@ swi_handler.S: SWI Handler
@
@ Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
@          Hui Jun Tay <htay@andrew.cmu.edu>
@ Date:    Tues, 11 Nov 2014 01:51:29 -0400

#include <arm/psr.h>

    .extern C_SWI_Handler

    .file   "swi_handler.S"
    .text

    .global swi_handler
swi_handler:
    stmfd   sp!, {lr}                    @ Push swi_irq
    stmfd   sp!, {r0-r3, r8, ip, lr}     @ Save caller saved regs.

    @ Save spsr. Saving user registers to emulate irq_handler.
    mrs     r0, spsr
    stmfd   sp, {r0, sp, lr}^
    sub     sp, sp, #12

    @ Restore U-Boot export table.
    ldr     ip, =global_data
    ldr     r8, [ip]

    @ Enable interrupts.
    msr     cpsr_c, #(PSR_MODE_SVC | PSR_FIQ)

    @ Call our C SWI handler
    ldr     r0, [lr, #-4]       @ Pass in swi number
    bic     r0, r0, #0xff000000

    add     r1, sp, #12         @ Pass in saved register array
                                @ Offset due to irq_handler emulation.
    bl      C_SWI_Handler       @ Jump to our C SWI handler.

    @ Restore spsr. Restoring user sp & lr due to launch_task clobber the
    @ user stack
    ldmfd   sp, {ip, sp, lr}^
    add     sp, sp, #12
    msr     spsr, ip

    @ Restore old registers and return.
    add     sp, sp, #4                      @ Bypass r0 in the stack.
    ldmfd   sp!, {r1-r3, r8, ip, lr, pc}^
