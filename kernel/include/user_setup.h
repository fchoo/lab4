/*
 * user_setup.h: Defines syscall numbers used in SWI instructions
 *
 * Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * Date:    Tues, 11 Nov 2014 01:51:29 -0400
 */

#ifndef USER_SETUP_H
#define USER_SETUP_H

int user_setup(int *sp_user);

#endif /* USER_SETUP_H */
