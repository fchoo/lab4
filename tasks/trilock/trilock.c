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
		printf("0 Start\n");
		printf("0 Get Lock\n");
        	mutex_lock(mutex);
		printf("0 Lock\n");

		putchar((int)str);
		printf("\n0 Unlock\n");
	        mutex_unlock(mutex);

		printf("0 Unlocked\n");
		printf("0 End\n");
		if (event_wait(3) < 0) panic("Dev 3 failed");
	}
}

void fun2(void* str) {
	while(1) {

		printf("2 Start\n");
		printf("2 Get Lock\n");
        	mutex_lock(mutex);
		printf("2 Lock\n");

		sleep(75);
		putchar((int)str);
		printf("\n2 Unlock\n");
	        mutex_unlock(mutex);
		printf("2 Unlocked\n");

		sleep(25);
		printf("2 End\n");

		if (event_wait(2) < 0) panic("Dev 2 failed");
	}
}

void fun3(void* str) {
	while(1) {

		printf("1 Start\n");
		printf("1 Get Lock\n");
        	mutex_lock(mutex);
		printf("1 Lock\n");

		sleep(75);
		putchar((int)str);
		printf("\n1 Unlock\n");
	        mutex_unlock(mutex);
		printf("1 Unlocked\n");

		printf("1 End\n");

		if (event_wait(0) < 0) panic("Dev 0 failed");
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
	tasks[0].B = 26;
	tasks[0].T = PERIOD_DEV3;
	tasks[1].lambda = fun2;
	tasks[1].data = (void*)'2';
	tasks[1].stack_pos = (void*)0xa1000000;
	tasks[1].C = 177;
    	tasks[1].B = 0;
	tasks[1].T = PERIOD_DEV2;
	tasks[2].lambda = fun3;
	tasks[2].data = (void*)'1';
	tasks[2].stack_pos = (void*)0xa1800000;
	tasks[2].C = 75;
    	tasks[2].B = 50;
	tasks[2].T = PERIOD_DEV0;
    mutex = mutex_create();

	task_create(tasks, 3);
	puts("Why did your code get here!\n");
	return 0;
}
