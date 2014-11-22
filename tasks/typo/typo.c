/** @file typo.c
 *
 * @brief Echos characters back with timing data.
 *
 * Links to libc.
 */

#include <unistd.h>
#include <stdio.h>

//times how long it takes for user to input and
//respond after a prompt
int main(int argc, char** argv)
{

	char buf[1024]; //arbitary buffer size
	const char prompt[] = "Please Enter a Message:\n";
	int size = 0;
	size_t curr = 0;
	size_t dur = 0;
	size_t now = 0;

	while(1) { //never terminate
	  //prompts user for input
	  write(STDOUT_FILENO, prompt, sizeof(prompt)-1);
	  curr = time(); //save time
	  size = read(STDIN_FILENO, buf, 1024);
	  write(STDOUT_FILENO, buf, size); //echo input
	  now = time();
	  dur = now - curr; //get duration
	  printf("%lu ms\n", dur); //print duration
	}
	return 0; //should never get here
}
