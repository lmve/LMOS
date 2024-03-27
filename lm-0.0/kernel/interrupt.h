#ifndef __KERNEL_INTERRUPT_H
#define __KERNEL_INTERRUPT_H
#include "stdint.h"
typedef void* intr_handler;		//将intr_handler定义为void*同类型
void idt_init(void);

/*添加ASSERT方便debug*/
/*
 *定义中断的两种状态
 *INTR_OFF    0 中断关闭
 *INTR_ON     1 中断开启
 */
enum intr_status{
	INTR_OFF,
	INTR_ON
};
/* 获取中断状态、设置中断状态、关闭中断、开启中断*/
enum intr_status intr_get_status(void);
enum intr_status intr_set_status(enum intr_status);
enum intr_status intr_enable(void);
enum intr_status intr_disable(void);

/* Add at 12.27*/
void register_handler(uint8_t vector_no, intr_handler function);
#endif
