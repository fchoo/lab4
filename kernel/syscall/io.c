/** @file io.c
 *
 * @brief Kernel I/O syscall implementations
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Sun, 14 Oct 2007 00:07:38 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-11-16
 */

#include <types.h>
#include <bits/errno.h>
#include <bits/fileno.h>
#include <arm/physmem.h>
#include <syscall.h>
#include <exports.h>
#include <kernel.h>

#define EOT_CHAR 0x04
#define DEL_CHAR 0x7f

#define SFROM_START 0x00000000
#define SFROM_END   0x00ffffff
#define SDRAM_START 0xa0000000
#define SDRAM_END   0xa3ffffff

/* Read count bytes (or less) from fd into the buffer buf. */
ssize_t read_syscall(int fd __attribute__((unused)), void *buf __attribute__((unused)), size_t count __attribute__((unused)))
{
    // Check for invalid memory range or file descriptors
    // Check if it does not override IRQ stack
    if (valid_addr((char *) buf, (int) count, SDRAM_START, SDRAM_END) == FALSE) {
        return -EFAULT;
    } else if (fd != STDIN_FILENO) {
        return -EBADF;
    }

    size_t i = 0;
    char *buffer = (char *) buf;
    char read_char;

    while (i < count) {
        read_char = getc();

        if (read_char == 4) { //EOT character
            return i;
        } else if (((read_char == 8) || (read_char == 127))) { // backspace or DEL character
            buffer[i] = 0; // '\0' character
            if(i > 0) {
                i--;
                puts("\b \b");
            }
        } else if ((read_char == 10) || (read_char == 13)) { // '\n' newline or '\r' carriage return character
            buffer[i] = '\n';
            putc('\n');
            return (i+1);
        } else {
            // put character into buffer and putc
            buffer[i] = read_char;
            i++;
            putc(read_char);
        }
    }

    return (ssize_t)i;


	
}

/* Write count bytes to fd from the buffer buf. */
ssize_t write_syscall(int fd  __attribute__((unused)), const void *buf  __attribute__((unused)), size_t count  __attribute__((unused)))
{

    // Check if it does not override IRQ stack
    // Check for invalid memory range or file descriptors
    if (valid_addr((char *) buf, (int) count, SDRAM_START, SDRAM_END) == FALSE &&
        valid_addr((char *) buf, (int) count, SFROM_START, SFROM_END) == FALSE) {
        return -EFAULT;
    } else if (fd != STDOUT_FILENO) {
        return -EBADF;
    }

    char *buffer = (char *) buf;
    size_t i;
    char read_char;
    for (i = 0; i < count; i++) {
        // put character into buffer and putc
        read_char = buffer[i];
        putc(read_char);
    }
    return (ssize_t)i;

}

