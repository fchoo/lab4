/** @file time.c
 *
 * @brief Kernel timer based syscall implementations
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-11-16
 */

#include <types.h>
#include <config.h>
#include <bits/errno.h>
#include <arm/timer.h>
#include <syscall.h>
#include <globals.h>

unsigned long time_syscall(void)
{
    unsigned long mstime = systime * TIMER_INTERVAL;
    return mstime;
}



/** @brief Waits in a tight loop for atleast the given number of milliseconds.
 *
 * @param millis  The number of milliseconds to sleep.
 *
 * 
 */
void sleep_syscall(unsigned long millis  __attribute__((unused)))
{
    unsigned long end = (millis/TIMER_INTERVAL);
    unsigned long currtime = systime;
    while(end > 0) {
      //decrease counter everytime systime changes by a unit
      if(systime > currtime) {
	  printf("%ds", end);
          end = end - 1;
          currtime = systime;
      }
    }
	
}
