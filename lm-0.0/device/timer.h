/*
 * 12.29 Add stdint.h mtime_sleep
 */
#ifndef __DEVICE_TIME_H
#define __DEVICE_TIME_H
#include "stdint.h"
void mtime_sleep(uint32_t m_seconds);
void timer_init(void);
#endif
