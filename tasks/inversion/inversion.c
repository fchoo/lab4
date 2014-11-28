/** @file inversion.c
 *
 * @brief tests priority inversion
 *
 * @author Hui Jun Tay, Foo Lai Choo (htay, fchoo)
 * @date   2014-11-25
 */
#include <stdio.h>
#include <task.h>
#include <unistd.h>


int mutex = 0;

void panic(const char* str)
{
	puts(str);
	while(1);
}

void fun1(void* str) {
	while (1) {
        	mutex_lock(mutex);
		sleep(25);

		putchar((int)str);

	        mutex_unlock(mutex);

		if (event_wait(0) < 0) panic("Dev 0 failed");
	}
}

void fun2(void* str) {
	while(1) {

        	mutex_lock(mutex);
		sleep(100);

		putchar((int)str);

	        mutex_unlock(mutex);

		if (event_wait(1) < 0) panic("Dev 1 failed");
	}
}

int main(int argc __attribute((unused)),
         char** argv __attribute((unused)))
{
	task_t tasks[2];
	tasks[0].lambda = fun1;
	tasks[0].data = (void*)'@';
	tasks[0].stack_pos = (void*)0xa2000000;
	tasks[0].C = 25;
	tasks[0].B = 0;
	tasks[0].T = PERIOD_DEV0;
	tasks[1].lambda = fun2;
	tasks[1].data = (void*)'<';
	tasks[1].stack_pos = (void*)0xa1000000;
	tasks[1].C = 100;
    	tasks[1].B = 50;
	tasks[1].T = PERIOD_DEV1;

    mutex = mutex_create();

	task_create(tasks, 2);
	puts("Why did your code get here!\n");
	return 0;
}
