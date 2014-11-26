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

//should print <#*@#>, since <* is in the lock statement

void fun1(void* str) {
	while (1) {
		putchar((int)'#');
        	mutex_lock(mutex);

		putchar((int)str);

	        mutex_unlock(mutex);
		putchar((int)'#');

		if (event_wait(3) < 0) panic("Dev 0 failed");
	}
}

void fun2(void* str) {
	while(1) {


		putchar((int)str);
		sleep(50);
        	mutex_lock(mutex);

		sleep(25);


		putchar((int)'*');

	        mutex_unlock(mutex);


		sleep(25);
		putchar((int)'>');
		if (event_wait(2) < 0) panic("Dev 1 failed");
	}
}

void fun3(void* str) {
	while(1) {

		putchar((int)'(');
        	mutex_lock(mutex);
		sleep(75);
		putchar((int)str);

        	mutex_unlock(mutex);
		putchar((int)')');
		if (event_wait(1) < 0) panic("Dev 1 failed");
	}
}
int main(int argc __attribute((unused)),
         char** argv __attribute((unused)))
{
	task_t tasks[3];
	tasks[0].lambda = fun1;
	tasks[0].data = (void*)'@';
	tasks[0].stack_pos = (void*)0xa2000000;
	tasks[0].C = 1;
	tasks[0].B = 26;
	tasks[0].T = PERIOD_DEV3;
	tasks[1].lambda = fun2;
	tasks[1].data = (void*)'<';
	tasks[1].stack_pos = (void*)0xa1000000;
	tasks[1].C = 177;
    	tasks[1].B = 0;
	tasks[1].T = PERIOD_DEV2;
	tasks[2].lambda = fun3;
	tasks[2].data = (void*)'!';
	tasks[2].stack_pos = (void*)0xa1800000;
	tasks[2].C = 75;
    	tasks[2].B = 50;
	tasks[2].T = PERIOD_DEV1;
    mutex = mutex_create();

	task_create(tasks, 3);
	puts("Why did your code get here!\n");
	return 0;
}
