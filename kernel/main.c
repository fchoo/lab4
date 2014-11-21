/*
 * main.c: Kernel main (entry) function
 *
 * Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * Date:    Tues, 11 Nov 2014 01:51:29 -0400
 */

#include <exports.h>

#include <arm/interrupt.h>
#include <arm/timer.h>
#include <arm/reg.h>

#include "include/globals.h"
#include "include/swi_handler.h"
#include "include/irq_handler.h"
#include "include/user_setup.h"
#include "include/config.h"

// 0xe59ff014 (LDR pc, [pc, 0x14]) --> 0x014 through masking
#define SWI_VECT_ADDR   0x08
#define IRQ_VECT_ADDR   0x18
#define PC_OFFSET       0x08

// (LDR pc, [pc, 0x000]) 0xe59ff000 --> 0x000 through masking
#define LDR_PC_PC_INSTR 0xe59ff000
#define LDR_SIGN_MASK   0x00800000
#define LDR_OFFSET_MASK 0x00000fff
#define LDR_PC_BACK1    0xe51ff004 // ldr pc, [pc, #-4]

#define BAD_CODE        0x0badc0de

/* Checks the SWI Vector Table. */
bool check_vector(int vect_addr) {
    int vector_instr = *((int *)vect_addr);

    // Check if the offset is negative.
    if ((vector_instr & LDR_SIGN_MASK) == 0) {
        return FALSE;
    }

    // Check that the instruction is a (LDR pc, [pc, 0x000])
    if (((uint32_t)vector_instr & ~LDR_OFFSET_MASK) != LDR_PC_PC_INSTR) {
        return FALSE;
    }

    return TRUE;
}

// Wire in the new handler and return the jmp table addr
int *wire_handler(int vec_addr, int new_handler_addr, int *old_instr_1, int *old_instr_2) {
    // Jump offset already incorporates PC offset. Usually 0x10 or 0x14.
    int jmp_offset = (*((int *) vec_addr))&(LDR_OFFSET_MASK);
    // &S_Handler" in Jump Table.
    int *handler_addr = *(int **)(vec_addr + PC_OFFSET + jmp_offset);
    // Save original Uboot handler instructions.
    *old_instr_1 = *handler_addr;
    *old_instr_2 = *(handler_addr + 1);
    // Wire in our own: LDR pc, [pc, #-4] = 0xe51ff004
    *handler_addr = LDR_PC_BACK1;
    *(handler_addr + 1) = new_handler_addr; // New handler.

    return handler_addr;
}

int kmain(int argc, char** argv, uint32_t table)
{
    app_startup();
    global_data = table;

    if (check_vector(SWI_VECT_ADDR) == FALSE) {
        return BAD_CODE;
    }
    /** Wire in the SWI handler. **/
    int swi_instr_1, swi_instr_2;
    int *swi_handler_addr = wire_handler(SWI_VECT_ADDR, (int)&swi_handler, &swi_instr_1, &swi_instr_2);

    if (check_vector(IRQ_VECT_ADDR) == FALSE) {
        return BAD_CODE;
    }
    /** Wire in the IRQ handler. **/
    int irq_instr_1, irq_instr_2;
    int *irq_handler_addr = wire_handler(IRQ_VECT_ADDR, (int)&irq_handler, &irq_instr_1, &irq_instr_2);

    // Copy argc and argv to user stack in the right order.
    int *spTop = ((int *) USR_STACK) - 1;
    int i = 0;
    for (i = argc-1; i >= 0; i--) {
        *spTop = (int)argv[i];
        spTop--;
    }
    *spTop = argc;

    //initialize systime to 0
    systime = 0;

    // initialize and mask OS Timer register bits
    reg_set(OSTMR_OIER_ADDR, OSTMR_OIER_E0); // Only activate timer 0
    reg_write(OSTMR_OSCR_ADDR, 0); // Init timer count to 0
    reg_write(OSTMR_OSMR_ADDR(0), TIMER_RES); // Set first match point

    reg_set(INT_ICMR_ADDR, 1<<INT_OSTMR_0);
    reg_clear(INT_ICLR_ADDR, 1<<INT_OSTMR_0);

    // TODO: Intialize timer and irq for dev

    /** Jump to user program. **/
    user_setup(spTop);

    // Note: We don't have to restore SWI/IRQ handlers since it will never come
    // back!

    return 0; // Should not reach here )=
}
