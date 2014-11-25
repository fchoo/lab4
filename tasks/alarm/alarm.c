/** @file alarm.c
 *
 * @brief Creates three periodic tasks that interrupt each other
 *
 * @note  
 *
 * @author Hui Jun Tay, Foo Lai Choo (htay, fchoo)
 * @date   2014-11-25
 */
#include <stdio.h>
#include <task.h>
#include <unistd.h>


void panic(const char* str)
{
	puts(str);
	while(1);
}

void tick(void* str)
{
	while(1)
	{
		putchar((int)str);
		if (event_wait(3) < 0)
			panic("(tick) dev 3 failed");
	}
}

void minute(void* str)
{
	while(1)
	{
		putchar((int)str);
		if (event_wait(0) < 0)
			panic("(min) dev 1 failed");
	}
}

void ring(void* str)
{
	while(1)
	{
		sleep(400);
		putchar((int)str);
		if (event_wait(2) < 0)
			panic("(ring) dev 2 failed");
	}
}

int main(int argc, char** argv)
{
	task_t tasks[2];
	//tasks[0].lambda = tick;
	//tasks[0].data = (void*)'-';
	//tasks[0].stack_pos = (void*)0xa2000000;
	//tasks[0].C = 1;
	//tasks[0].T = PERIOD_DEV3;
	tasks[0].lambda = minute;
	tasks[0].data = (void*)'+';
	tasks[0].stack_pos = (void*)0xa1000000;
	tasks[0].C = 10;
	tasks[0].B = 1;
	tasks[0].T = PERIOD_DEV0;
	tasks[1].lambda = ring;
	tasks[1].data = (void*)'@';
	tasks[1].stack_pos = (void*)0xa2800000;
	tasks[1].C = 400;
	tasks[1].B = 10;
	tasks[1].T = PERIOD_DEV2;
	task_create(tasks, 2);
	argc=argc; /* remove compiler warning */
	argv[0]=argv[0]; /* remove compiler warning */

	puts("Why did your code get here!\n");
	return 0;
}
