/** @file splat.c
 * 
 * @brief Displays a spinning cursor.
 *
 * Links to libc.
 */

#include <unistd.h>
#include <stdio.h>

#define SLEEP_CYCLE 200
int main(int argc, char** argv)
{
	int pos = 0;
	char c[] = "|"; //start at |
	char backspace[] = "\b \b";
	while(1) { //never terminate
	    //wait 200ms between "rotations"to show the bar
	    write(STDOUT_FILENO, c, 1);
	    sleep(SLEEP_CYCLE);
		
	    pos++; //next position
	    pos = pos % 4; //limit to 4 positions

	    //case output on 'frame'
	    switch(pos) {
	      case 0:
		c[0] = '|'; 
	      break;
	      case 1:
		c[0] = '/'; 
	      break;
	      case 2:
		c[0] = '-'; 
	      break;
	      case 3:
		c[0] = '\\'; 
	      break;
	      default:
 		c[0] = 'o'; //debug value 
	      break;
	   } 
	    //backspace to delete the bar, given illusion of 'rotation'
	    write(STDOUT_FILENO, backspace, sizeof(backspace) -1);
	  
	}

	return 0; //should never get here
}
