/** @file kernel.h
 *
 * @brief Main kernel -- primary interface.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-11-17
 */

#ifndef KERNEL_H
#define KERNEL_H

#include <types.h>

int enter_user_mode(void);
int valid_addr(const void* start, size_t count, uintptr_t base, uintptr_t bound);
int is_userSpace(uint32_t addr);
int is_stackAligned(uint32_t addr);

#endif /* KERNEL_H */
