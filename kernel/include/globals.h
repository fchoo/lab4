/*
 * globals.h: Defines global variables for the program.
 *
 * Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * Date:    Tues, 11 Nov 2014 01:51:29 -0400
 */

#ifndef GLOBALS_H
#define GLOBALS_H

typedef enum {FALSE, TRUE} bool;
extern uint32_t global_data;
extern volatile unsigned long systime;

#endif /* GLOBALS_H */
