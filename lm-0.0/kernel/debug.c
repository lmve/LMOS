/*
 *  debug.c        kernel/debug.c
 *  (c)  2023    lucky_ma
 *
 *  12.26 ADD panic_spin
 *    打印异常信息
 */

#include "debug.h"
#include "print.h"
#include "interrupt.h"
/* 在interrupt 中定义了开关中断的函数 */

/* 打印文件名，行号，函数名，条件等信息，并使程序停止 */
void panic_spin(char* filename, int line, const char* func, const char* condition){
	intr_disable(); /* 当程序发生 panic 时，先关闭中断[1] */
	put_str("\n\n   error    \n"); /* 打印错误详细信息[2] */
	put_str("filename:");
	put_str(filename);
	put_str("\n");
	put_str("line:0x");
	put_int(line);
	put_str("\n");
	put_str("function:");
	put_str((char*)func);   
	put_str("\n");
    put_str("condition:");
	put_str((char*)condition);
	put_str("\n");
	while(1);
}




