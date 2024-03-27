/*
 * Create at 12.28
 */
#ifndef __USERPROG_SYSCALLINIT_H
#define __USERPROG_SYSCALLINIT_H
#include "stdint.h"
void syscall_init(void);
uint32_t sys_getpid(void);
/* After fs uint32_t sys_write(char* str); */
#endif
