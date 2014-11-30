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

//test that midlevel task 1 does not interrupt low level task 2 when
//it holds the lock for high level task 0
//brackets indicate the start and end of a function, before/after it lock/unlocks
//the prio of the func is printed while inside the lock statement
//should print <(20)>

void fun1(void* str) {
	while (1) {
		putchar((int)'(');
        	mutex_lock(mutex);

		putchar((int)str);

	        mutex_unlock(mutex);
		putchar((int)')');

		if (event_wait(0) < 0) panic("Dev 0 failed");
	}
}

void fun2(void* str) {
	while(1) {


        	mutex_lock(mutex);
		putchar((int)'<');
		sleep(100);


		putchar((int)str);

	        mutex_unlock(mutex);
		sleep(50);


		putchar((int)'>');
		if (event_wait(2) < 0) panic("Dev 1 failed");
	}
}

void fun3(void* str) {
	while(1) {

		sleep(10);
		putchar((int)str);

		if (event_wait(1) < 0) panic("Dev 1 failed");
	}
}
int main(int argc __attribute((unused)),
         char** argv __attribute((unused)))
{
	task_t tasks[3];
	tasks[0].lambda = fun1;
	tasks[0].data = (void*)'0';
	tasks[0].stack_pos = (void*)0xa2000000;
	tasks[0].C = 1;
	tasks[0].B = 0;
	tasks[0].T = PERIOD_DEV0;
	tasks[1].lambda = fun2;
	tasks[1].data = (void*)'2';
	tasks[1].stack_pos = (void*)0xa1000000;
	tasks[1].C = 165;
    	tasks[1].B = 100;
	tasks[1].T = PERIOD_DEV2;
	tasks[2].lambda = fun3;
	tasks[2].data = (void*)'1';
	tasks[2].stack_pos = (void*)0xa1800000;
	tasks[2].C = 10;
    	tasks[2].B = 100;
	tasks[2].T = PERIOD_DEV1;
    mutex = mutex_create();

	task_create(tasks, 3);
	puts("Why did your code get here!\n");
	return 0;
}
