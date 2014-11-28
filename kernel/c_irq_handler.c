/*
 * kernel.c: Kernel main (entry) function
 *
 * Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * Date:    Tues, 11 Nov 2014 01:51:29 -0400
 */

#include <exports.h>
#include <types.h>
#include <arm/reg.h>
#include <arm/timer.h>
#include <arm/interrupt.h>
#include <syscall.h>
#include <globals.h>
#include <config.h>
#include <device.h>

#define DEV_INTERVAL 5

/* C_IRQ_Handler */
void C_IRQ_Handler() {
    // Get OSMR0 bit
    uint32_t bit_OSMR0 = 1 << (INT_OSTMR_0);
    uint32_t has_OSMR0 = reg_read(INT_ICPR_ADDR) & bit_OSMR0;
    uint32_t next_match;
    // if OMSR0 is set
    if (has_OSMR0 == bit_OSMR0) {
        // Current count + timer res = next match point
        next_match = reg_read(OSTMR_OSCR_ADDR) + OS_TIMER_RESOLUTION;
        reg_write(OSTMR_OSMR_ADDR(0), next_match);
        // Clear OSSR match
        reg_set(OSTMR_OSSR_ADDR, OSTMR_OSSR_M0);
        // Update system time
        systime++;
    }
    // Update only every 50ms
    if ((systime - dev_offset) % DEV_INTERVAL == 0) {
        dev_update(systime * TIMER_INTERVAL);
    }
}
