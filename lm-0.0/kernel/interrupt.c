/*
 *  interrupt.c        kernel/interrupt.c
 *  (c)  2023    lucky_ma
 *
 *  12.26     ADD assert 是为了实现中断的随时开关
 *    有些时候当程序 panic 在输出错误信息时不想被其他信息干扰
 *  12.27    ADD register_handler
 *  12.29    Revised pic_init() open irq14 line interruption
 */

/* changed  12.27 add register_handler
    修改通用中断处理函数general_intr_handler，增加修改光标位置打印错误新的的代码，
    原因是多线程切换引发的同步问题，有时候会引发光标值超过允许范围，这个范围是显存段段描述符的界限决定的，
    如果这时候由光标值超限引发的中断，然后调用general_intr_handler去打印信息，将再次导致异常，那么就不会输出错误信息；
*/
#include "interrupt.h"      //里面定义了intr_handler类型
#include "stdint.h"         //各种uint_t类型
#include "global.h"         //里面定义了选择子
#include "io.h"             
#include "print.h"


#define PIC_M_CTRL 0x20	       // 这里用的可编程中断控制器是8259A,主片的控制端口是0x20
#define PIC_M_DATA 0x21	       // 主片的数据端口是0x21
#define PIC_S_CTRL 0xa0	       // 从片的控制端口是0xa0
#define PIC_S_DATA 0xa1	       // 从片的数据端口是0xa1


//#define IDT_DESC_CNT 0x21	   //支持的中断描述符个数33
/* Changed at 12.28 */
#define IDT_DESC_CNT 0x81      // 目前总共支持的中断数，最后一个支持的中断号0x80 + 1
extern uint32_t syscall_handler(void);    //定义的汇编中断处理程序代码

/*ASSERT 12.26*/
#define EFLAGS_IF    0x00000200  //将eflags寄存器中的if位设为1
#define GET_EFLAGS(EFLAGS_VAR) asm volatile("pushfl; popl %0" : "=g" (EFLAGS_VAR))
/*pop到EFLAG_VAR对应的内存中，g表示任意寄存器，内存或立即数*/



//按照中断门描述符格式定义结构体
struct gate_desc {
   uint16_t    func_offset_low_word;        //函数地址低字
   uint16_t    selector;                    //选择子字段
   uint8_t     dcount;                      //此项为双字计数字段，是门描述符中的第4字节。这个字段无用
   uint8_t     attribute;                   //属性字段
   uint16_t    func_offset_high_word;       //函数地址高字
};

// 静态函数声明,非必须
static void make_idt_desc(struct gate_desc* p_gdesc, uint8_t attr, intr_handler function);
static struct gate_desc idt[IDT_DESC_CNT];   //中断门描述符（结构体）数组，名字叫idt

extern intr_handler intr_entry_table[IDT_DESC_CNT];	    //引入kernel.s中定义好的中断处理函数地址数组，intr_handler就是void* 表明是一般地址类型

char* intr_name[IDT_DESC_CNT];		         //存储中断/异常的名字
intr_handler idt_table[IDT_DESC_CNT];	     // 定义中断处理程序数组.在kernel.S中定义的intrXXentry只是中断处理程序的入口,最终调用的是ide_table中的处理程序

/* 初始化可编程中断控制器8259A */
static void pic_init(void) {

   /* 初始化主片 */
   outb (PIC_M_CTRL, 0x11);   // ICW1: 边沿触发,级联8259, 需要ICW4.
   outb (PIC_M_DATA, 0x20);   // ICW2: 起始中断向量号为0x20,也就是IR[0-7] 为 0x20 ~ 0x27.
   outb (PIC_M_DATA, 0x04);   // ICW3: IR2接从片. 
   outb (PIC_M_DATA, 0x01);   // ICW4: 8086模式, 正常EOI

   /* 初始化从片 */
   outb (PIC_S_CTRL, 0x11);	// ICW1: 边沿触发,级联8259, 需要ICW4.
   outb (PIC_S_DATA, 0x28);	// ICW2: 起始中断向量号为0x28,也就是IR[8-15] 为 0x28 ~ 0x2F.
   outb (PIC_S_DATA, 0x02);	// ICW3: 设置从片连接到主片的IR2引脚
   outb (PIC_S_DATA, 0x01);	// ICW4: 8086模式, 正常EOI

   /* Changed at 12.28 */
   /* 打开主片上IR0,也就是目前只接受时钟产生的中断 */
   //outb (PIC_M_DATA, 0xfe);
   //outb (PIC_S_DATA, 0xff);

   /* 测试键盘,只打开键盘中断，其它全部关闭 */
   //outb (PIC_M_DATA, 0xfd);   //键盘中断在主片ir1引脚上，所以将这个引脚置0，就打开了
   //outb (PIC_S_DATA, 0xff);

   /* Add at 12.28 */
   //同时打开时钟中断与键盘中断
   //outb (PIC_M_DATA, 0xfc);
   //outb (PIC_S_DATA, 0xff);

   /* revised at 12.29 */
   outb (PIC_M_DATA, 0xf8);    //IRQ2用于级联从片,必须打开,否则无法响应从片上的中断主片上打开的中断有IRQ0的时钟,IRQ1的键盘和级联从片的IRQ2,其它全部关闭
   outb (PIC_S_DATA, 0xbf);    //打开从片上的IRQ14,此引脚接收硬盘控制器的中断 
   put_str("   pic_init done\n");
}


//此函数用于将传入的中断门描述符与中断处理函数建立映射，三个参数：中断门描述符地址，属性，中断处理函数地址
static void make_idt_desc(struct gate_desc* p_gdesc, uint8_t attr, intr_handler function) { 
   p_gdesc->func_offset_low_word = (uint32_t)function & 0x0000FFFF;
   p_gdesc->selector = SELECTOR_K_CODE;
   p_gdesc->dcount = 0;
   p_gdesc->attribute = attr;
   p_gdesc->func_offset_high_word = ((uint32_t)function & 0xFFFF0000) >> 16;
}

//此函数用来循环调用make_idt_desc函数来完成中断门描述符与中断处理函数映射关系的建立,传入三个参数：中断描述符表某个中段描述符（一个结构体）的地址
//属性字段，中断处理函数的地址
//Changed at 12.28 
static void idt_desc_init(void) {
   int i, lastindex = IDT_DESC_CNT - 1;
   for (i = 0; i < IDT_DESC_CNT; i++) {
      make_idt_desc(&idt[i], IDT_DESC_ATTR_DPL0, intr_entry_table[i]); 
   }
   //单独处理系统调用,系统调用对应的中断门dpl为3,中断处理程序为汇编的syscall_handler
   make_idt_desc(&idt[lastindex], IDT_DESC_ATTR_DPL3, syscall_handler);
   put_str("   idt_desc_init done\n");
}

/* Changed at 12.27 */
/* 通用的中断处理函数,用于初始化,一般用在异常出现时的处理 */
static void general_intr_handler(uint8_t vec_nr) {
   if (vec_nr == 0x27 || vec_nr == 0x2f) {	//伪中断向量，无需处理。详见书p337
      return;		
   }
    /* 将光标置为0,从屏幕左上角清出一片打印异常信息的区域,方便阅读 */
   set_cursor(0);
   int cursor_pos = 0;
   while(cursor_pos < 320){
      put_char(' ');
      cursor_pos++;
   }
   set_cursor(0);	      // 重置光标为屏幕左上角
   put_str("!!!!!!!      excetion message begin  !!!!!!!!\n");
   set_cursor(88);	   // 从第2行第8个字符开始打印
   put_str(intr_name[vec_nr]);
   if (vec_nr == 14) {	  // 若为Pagefault,将缺失的地址打印出来并悬停
      int page_fault_vaddr = 0; 
      asm ("movl %%cr2, %0" : "=r" (page_fault_vaddr));	  // cr2是存放造成page_fault的地址
      put_str("\npage fault addr is ");put_int(page_fault_vaddr); 
   }
   put_str("\n!!!!!!!      excetion message end    !!!!!!!!\n");
  // 能进入中断处理程序就表示已经处在关中断情况下,
  // 不会出现调度进程的情况。故下面的死循环不会再被中断。
   while(1);
}

/* 完成一般中断处理函数注册及异常名称注册 */
static void exception_init(void) {			    // 完成一般中断处理函数注册及异常名称注册
   int i;
   for (i = 0; i < IDT_DESC_CNT; i++) {

/* idt_table数组中的函数是在进入中断后根据中断向量号调用的,
 * 见kernel/kernel.S的call [idt_table + %1*4] */
      idt_table[i] = general_intr_handler;		    // 默认为general_intr_handler。
							    // 以后会由register_handler来注册具体处理函数。
      intr_name[i] = "unknown";				    // 先统一赋值为unknown 
   }
   intr_name[0] = "#DE Divide Error";
   intr_name[1] = "#DB Debug Exception";
   intr_name[2] = "NMI Interrupt";
   intr_name[3] = "#BP Breakpoint Exception";
   intr_name[4] = "#OF Overflow Exception";
   intr_name[5] = "#BR BOUND Range Exceeded Exception";
   intr_name[6] = "#UD Invalid Opcode Exception";
   intr_name[7] = "#NM Device Not Available Exception";
   intr_name[8] = "#DF Double Fault Exception";
   intr_name[9] = "Coprocessor Segment Overrun";
   intr_name[10] = "#TS Invalid TSS Exception";
   intr_name[11] = "#NP Segment Not Present";
   intr_name[12] = "#SS Stack Fault Exception";
   intr_name[13] = "#GP General Protection Exception";
   intr_name[14] = "#PF Page-Fault Exception";
   // intr_name[15] 第15项是intel保留项，未使用
   intr_name[16] = "#MF x87 FPU Floating-Point Error";
   intr_name[17] = "#AC Alignment Check Exception";
   intr_name[18] = "#MC Machine-Check Exception";
   intr_name[19] = "#XF SIMD Floating-Point Exception";

}

/*完成有关中断的所有初始化工作*/
void idt_init() {
   put_str("idt_init start\n");
   idt_desc_init();	   //调用上面写好的函数完成中段描述符表的构建
   exception_init();	   // 异常名初始化并注册通常的中断处理函数
   pic_init();		  //设定化中断控制器，只接受来自时钟中断的信号

   /* 加载idt */
   uint64_t idt_operand = ((sizeof(idt) - 1) | ((uint64_t)(uint32_t)idt << 16));    //定义要加载到IDTR寄存器中的值
   asm volatile("lidt %0" : : "m" (idt_operand));
   put_str("idt_init done\n");
}

/*ADD ASSERT 12.26*/
/* 获取当前中断状态 */
enum intr_status intr_get_status(){
	uint32_t eflags = 0;
	GET_EFLAGS(eflags);
	return (EFLAGS_IF & eflags) ? INTR_ON : INTR_OFF;
}

/* 开中断并返回开之前的状态 */
enum intr_status intr_enable(){
	enum intr_status old_status;
	if(INTR_ON == intr_get_status()){
		old_status = INTR_ON;
		return old_status;
	} else {
		old_status = INTR_OFF;
		/* sti 指令让 IF 位为1*/
		asm volatile("sti");  
		return old_status;
	}
}

/* 关中断并返回中断之前的状态 */
enum intr_status intr_disable(){
   enum intr_status old_status;
   if(INTR_ON == intr_get_status()){
      old_status = INTR_ON;
      asm volatile("cli":::"memory"); /* 关中断 cli 指令将 if 位置0*/
      return old_status;
   } else {
      old_status = INTR_OFF;
      return old_status;
   }
}

/* Add at 12.27 */
/* 在中断处理程序数组第vector_no个元素中注册安装中断处理程序function */
void register_handler(uint8_t vector_no, intr_handler function) {
/* idt_table数组中的函数是在进入中断后根据中断向量号调用的,
 * 见kernel/kernel.S的call [idt_table + %1*4] */
   idt_table[vector_no] = function; 
}

/* 中断状态设置为 status */
enum intr_status intr_set_status(enum intr_status status){
   return status & INTR_ON ? intr_enable() : intr_disable();
}