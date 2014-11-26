/** @file dev_multask.c
 *
 * @brief Testing multiple tasks in device
 *
 * @note  This is like knife -- but smaller :)
 *
 * Authors: Foo Lai Choo <fchoo@andrew.cmu.edu>
 *          Hui Jun Tay <htay@andrew.cmu.edu>
 * Date:    Tues, 11 Nov 2014 01:51:29 -0400
 */
#include <stdio.h>
#include <task.h>
#include <unistd.h>


void panic(const char* str)
{
	puts(str);
	while(1);
}

void fun0(void* str)
{
	while(1)
	{
		putchar((int)str);
		if (event_wait(0) < 0)
			panic("Dev 0 failed");
	}
}

void fun1(void* str)
{
	while(1)
	{
		putchar((int)str);
		if (event_wait(1) < 0)
			panic("Dev 1 failed");
	}
}

int main(int argc __attribute((unused)), char** argv __attribute((unused)))
{
	task_t tasks[3];
	tasks[0].lambda = fun0;
	tasks[0].data = (void*)'0';
	tasks[0].stack_pos = (void*)0xa0001000;
	tasks[0].C = 20;
	tasks[0].T = PERIOD_DEV0; // 100
	tasks[0].B = 1;
	tasks[1].lambda = fun0;
	tasks[1].data = (void*)'1';
	tasks[1].stack_pos = (void*)0xa0002000;
	tasks[1].C = 20;
	tasks[1].T = PERIOD_DEV0; // 100
	tasks[1].B = 1;
	tasks[2].lambda = fun1;
	tasks[2].data = (void*)'2';
	tasks[2].stack_pos = (void*)0xa0003000;
	tasks[2].C = 40;
	tasks[2].T = PERIOD_DEV1; // 200
	tasks[2].B = 1;
	tasks[3].lambda = fun1;
	tasks[3].data = (void*)'3';
	tasks[3].stack_pos = (void*)0xa0004000;
	tasks[3].C = 100;
	tasks[3].T = PERIOD_DEV1; // 200
	tasks[3].B = 1;

	task_create(tasks, 4);

	puts("Why did your code get here!\n");
	return 0;
}
