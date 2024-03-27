#include "print.h"
#include "init.h"
#include "debug.h"
#include "../lib/user/string.h"
#include "memory.h"
#include "thread.h"


void k_thread_a(void*);
void k_thread_b(void*);


int main(void) {
   put_str("I am kernel\n");
   init_all();
  // asm volatile("sti");	     // 为演示中断处理,在此临时开中断
   ASSERT(2==2);
  /*
   put_str("memory test\n");     // 内存测试
   char mem[10];
   void* test = mem;
   memset(test,0,10);
   memset(test,64,9);
   put_str(test);
 
   put_str("string test\n");    // 字符串测试
   int r = 2;
   char a[] = "abc";
   char b[] = "def";
   char* c = strcat(a,b);
   put_str("\n");
   put_str(c);
   put_str("all test done!\n");
   */
   
   /* bitmap management test */
   /*
   int i;
   struct bitmap btm;                              //创建管理位图的数据结构
   struct bitmap* bitmap = &btm;
   uint8_t test_map[2];                            //用这个数组充当测试用的位图，两字节16位
   bitmap->btmp_bytes_len = sizeof(test_map);       //bitmap的长度域填入整个位图的长度
   bitmap->bits = test_map;                        //bitmap的地址域填入位图的起始地址
   bitmap_init(bitmap);                             //位图初始化，实际效果就是位图的所有位都变成0
                                                   //以下是在设置位图，设置成1111 0011 0001 0000
   for(i = 0; i < 4; i++){
      bitmap_set(bitmap, i, 1);
   }
   bitmap_set(bitmap, 6, 1);
   bitmap_set(bitmap, 7, 1);
   bitmap_set(bitmap, 11, 1);
   put_char('\n');
   for(i = 0; i < 16; i++){
      put_int(bitmap_scan_test(bitmap, i));
   }
   put_char('\n');
   put_int(bitmap_scan(bitmap, 4));              //检验bitmap_scan
   put_char('\n');
   put_int(bitmap_scan(bitmap, 5));
   */
   
   /* memory management test*/
   /*
   put_str("I am kernel\n");
   init_all();

   void* addr = get_kernel_pages(3);
   put_str("\n get_kernel_page start vaddr is ");
   put_int((uint32_t)addr);
   put_str("\n");
   while(1);
   */
    
   /* thread test */ 
    int i = 9;
   thread_start("k_thread_a", 31, k_thread_a, "argA ");
   thread_start("k_thread_b", 31, k_thread_b, "argB ");

   intr_enable();	// 打开中断,使时钟中断起作用
    while(1)
    {
        while(i--);
        i=9;
        intr_disable();
        put_str("main ");
        intr_enable();
    }   
   return 0;
}


/*
 * Add at 12.27
 *
 */
/* 在线程中运行的函数 */
void k_thread_a(void* arg) {     
/* 用void*来通用表示参数,被调用的函数知道自己需要什么类型的参数,自己转换再用 */
    int i=9;
    char* tmp = arg;
    while(1)
    {
        while(i--);
        i=9;
        intr_disable();
        put_str(tmp);  
        intr_enable();      
    }
}

/* 在线程中运行的函数 */
void k_thread_b(void* arg) {     
/* 用void*来通用表示参数,被调用的函数知道自己需要什么类型的参数,自己转换再用 */
    int i=9;
    char* tmp = arg;
    while(1)
    {
        while(i--);
        i=9;
        intr_disable();
        put_str(tmp);
        intr_enable();
    }
}







/* before  12.29  */
#include "thread.h"
#include "interrupt.h"
#include "console.h"
#include "process.h"
#include "syscall-init.h"
#include "syscall.h"
#include "stdio.h"
#include "memory.h"

void k_thread_a(void*);
void k_thread_b(void*);
void u_prog_a(void);
void u_prog_b(void);

int main(void) {
   put_str("I am kernel\n");
   init_all();
   intr_enable();
   process_execute(u_prog_a, "u_prog_a");
   process_execute(u_prog_b, "u_prog_b");
   thread_start("k_thread_a", 31, k_thread_a, "I am thread_a");
   thread_start("k_thread_b", 31, k_thread_b, "I am thread_b");
   while(1);
   return 0;
}

/* 在线程中运行的函数 */
void k_thread_a(void* arg) {     
   void* addr1 = sys_malloc(256);
   void* addr2 = sys_malloc(255);
   void* addr3 = sys_malloc(254);
   console_put_str(" thread_a malloc addr:0x");
   console_put_int((int)addr1);
   console_put_char(',');
   console_put_int((int)addr2);
   console_put_char(',');
   console_put_int((int)addr3);
   console_put_char('\n');

   int cpu_delay = 9999999;
   while(cpu_delay-- > 0);
   sys_free(addr1);
   sys_free(addr2);
   sys_free(addr3);
   while(1);
}

/* 在线程中运行的函数 */
void k_thread_b(void* arg) {     
   void* addr1 = sys_malloc(256);
   void* addr2 = sys_malloc(255);
   void* addr3 = sys_malloc(254);
   console_put_str(" thread_b malloc addr:0x");
   console_put_int((int)addr1);
   console_put_char(',');
   console_put_int((int)addr2);
   console_put_char(',');
   console_put_int((int)addr3);
   console_put_char('\n');

   int cpu_delay = 999999;
   while(cpu_delay-- > 0);
   sys_free(addr1);
   sys_free(addr2);
   sys_free(addr3);
   while(1);
}

/* 测试用户进程 */
void u_prog_a(void) {
   void* addr1 = malloc(256);
   void* addr2 = malloc(255);
   void* addr3 = malloc(254);
   printf(" prog_a malloc addr:0x%x,0x%x,0x%x\n", (int)addr1, (int)addr2, (int)addr3);

   int cpu_delay = 100000;
   while(cpu_delay-- > 0);
   free(addr1);
   free(addr2);
   free(addr3);
   while(1);
}

/* 测试用户进程 */
void u_prog_b(void) {
   void* addr1 = malloc(256);
   void* addr2 = malloc(255);
   void* addr3 = malloc(254);
   printf(" prog_b malloc addr:0x%x,0x%x,0x%x\n", (int)addr1, (int)addr2, (int)addr3);

   int cpu_delay = 100000;
   while(cpu_delay-- > 0);
   free(addr1);
   free(addr2);
   free(addr3);
   while(1);
}


/* before fs */
#include "print.h"
#include "init.h"
#include "thread.h"
#include "interrupt.h"
#include "console.h"
#include "process.h"
#include "syscall-init.h"
#include "syscall.h"
#include "stdio.h"
#include "memory.h"

void k_thread_a(void*);
void k_thread_b(void*);
void u_prog_a(void);
void u_prog_b(void);

int main(void) {
   put_str("I am kernel\n");
   init_all();
   while(1);
   process_execute(u_prog_a, "u_prog_a");
   process_execute(u_prog_b, "u_prog_b");
   thread_start("k_thread_a", 31, k_thread_a, "I am thread_a");
   thread_start("k_thread_b", 31, k_thread_b, "I am thread_b");
   while(1);
   return 0;
}

/* 在线程中运行的函数 */
void k_thread_a(void* arg) {     
   void* addr1 = sys_malloc(256);
   void* addr2 = sys_malloc(255);
   void* addr3 = sys_malloc(254);
   console_put_str(" thread_a malloc addr:0x");
   console_put_int((int)addr1);
   console_put_char(',');
   console_put_int((int)addr2);
   console_put_char(',');
   console_put_int((int)addr3);
   console_put_char('\n');

   int cpu_delay = 100000;
   while(cpu_delay-- > 0);
   sys_free(addr1);
   sys_free(addr2);
   sys_free(addr3);
   while(1);
}

/* 在线程中运行的函数 */
void k_thread_b(void* arg) {     
   void* addr1 = sys_malloc(256);
   void* addr2 = sys_malloc(255);
   void* addr3 = sys_malloc(254);
   console_put_str(" thread_b malloc addr:0x");
   console_put_int((int)addr1);
   console_put_char(',');
   console_put_int((int)addr2);
   console_put_char(',');
   console_put_int((int)addr3);
   console_put_char('\n');

   int cpu_delay = 100000;
   while(cpu_delay-- > 0);
   sys_free(addr1);
   sys_free(addr2);
   sys_free(addr3);
   while(1);
}

/* 测试用户进程 */
void u_prog_a(void) {
   void* addr1 = malloc(256);
   void* addr2 = malloc(255);
   void* addr3 = malloc(254);
   printf(" prog_a malloc addr:0x%x,0x%x,0x%x\n", (int)addr1, (int)addr2, (int)addr3);

   int cpu_delay = 100000;
   while(cpu_delay-- > 0);
   free(addr1);
   free(addr2);
   free(addr3);
   while(1);
}

/* 测试用户进程 */
void u_prog_b(void) {
   void* addr1 = malloc(256);
   void* addr2 = malloc(255);
   void* addr3 = malloc(254);
   printf(" prog_b malloc addr:0x%x,0x%x,0x%x\n", (int)addr1, (int)addr2, (int)addr3);

   int cpu_delay = 100000;
   while(cpu_delay-- > 0);
   free(addr1);
   free(addr2);
   free(addr3);
   while(1);
}

/* 12.30 before */

#include "print.h"
#include "init.h"
#include "fs.h"
#include "stdio.h"
#include "syscall.h"


void init(void);
int main(void)
{
    put_str("I am kernel\n");
    init_all();
    
    /********  测试代码  ********/
    /*
    struct stat obj_stat;
    sys_stat("/", &obj_stat);
    printf("/`s info\n   i_no:%d\n   size:%d\n   filetype:%s\n",
           obj_stat.st_ino, obj_stat.st_size,
           obj_stat.st_filetype == 2 ? "directory" : "regular");
    sys_stat("/dir1", &obj_stat);
    printf("/dir1`s info\n   i_no:%d\n   size:%d\n   filetype:%s\n",
           obj_stat.st_ino, obj_stat.st_size,
           obj_stat.st_filetype == 2 ? "directory" : "regular");
          
    /********  测试代码  ********/
    
    while (1);
    return 0;
}


/* init进程 */
void init(void)
{
    uint32_t ret_pid = fork();
    if (ret_pid)
    {
        printf("i am father, my pid is %d, child pid is %d\n", getpid(), ret_pid);
    }
    else
    {
        printf("i am child, my pid is %d, ret pid is %d\n", getpid(), ret_pid);
    }
    while (1);
}


finall:
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

