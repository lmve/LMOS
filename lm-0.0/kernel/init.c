/*
 *    init.c        kernel/init.c
 *    (c)  2023    lucky_ma 
 */
 
/* 12.26 add mem_init */
/* 12.27 add thread_init */
/* 12.28 add console_init *
 *       add keyboard_init *
 *       tss_init *
 *       syscall_init *
 * 12.29 add ide.h
 *       add fs.h
 */

#include "init.h"
#include "print.h"
#include "interrupt.h"
#include "timer.h"
#include "memory.h"
#include "thread.h"
#include "console.h"
#include "keyboard.h"
#include "tss.h"
#include "syscall-init.h"
#include "ide.h"
#include "fs.h"

/*负责初始化所有模块 */
void init_all() {
   put_str("init_all\n");
   idt_init();   // 初始化中断
   mem_init();   // 初始化内存管理系统
   thread_init(); //初始化线程相关结构
   timer_init(); 
   console_init();//最好在开中断以前
   keyboard_init();//键盘初始化
   tss_init();    //tss初始化
   syscall_init();//初始化系统调用
   ide_init();	     // 初始化硬盘
   filesys_init();   // 初始化文件系统
   
   
}
