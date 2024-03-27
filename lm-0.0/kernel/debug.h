#ifndef __KERNEL_DEBUG_H
#define __KERNEL_DEBUG_H

void panic_spin(char* filename, int line, const char* func, const char* condition);
/* ... 是参数个数可变的宏，将会传到 __VA_ARGS__
 * __FILE__,__LINE__,__func__都是预定于宏，代表这个宏所在的文件名，行数，函数
 */
#define PANIC(...) panic_spin(__FILE__,__LINE__,__func__,__VA_ARGS__)

/* if define NDEBUG then ASSERT is empty value.
 * ASSERT macor will not work
 * if we don't want it work 
 * use command gcc -DNDEBUG
 * well, everything will be ok!
 */

#ifdef NDEBUG
    #define ASSERT(CONDITION) ((void)0)
#else
    #define ASSERT(CONDITION) \
	if(CONDITION){}           \
    else{PANIC(#CONDITION);}   /* 加 # 是将参数变为字符串 */
#endif  /* 结束 NDEBUG */
#endif
