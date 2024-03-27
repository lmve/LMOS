/* 
 * main           kernel/main
 * (c)  2023    lucky_ma
 *
 * 12.26  test ASSERT add debug.h
 *        test memory & string operate function
 *        test memory management 
 * 12.27  test thread
 * 12.29  test ide
 * 12.29  test fs
 * 12.30  test fork
 */
#include "print.h"
#include "init.h"
#include "fork.h"
#include "stdio.h"
#include "syscall.h"
#include "debug.h"
#include "shell.h"
#include "console.h"

void init(void); //在thread/thread.c/thread_init() 中创init进程

int main(void) {
   put_str("I am kernel\n");
   init_all();
   //cls_screen();
   put_str("Welcome to lmos\n");
   //console_put_str("[lm-0.0@localhost /]$ ");
   
   my_shell();
   return 0;
}
/* init进程 */
void init(void)
{
    uint32_t ret_pid = fork();
    if (ret_pid)
    { // 父进程
        printf(" Im father ,mypid is %d,my chiled is %d",getpid(),ret_pid);
        while (1)
            ;
    }
    else
    { // 子进程
        printf(" Im child ,mypid is %d,my father is %d",ret_pid,getpid());
        my_shell();
    }
    PANIC("init: should not be here");
}

