#ifndef __XPT2046_H
#define __XPT2046_H

#include <stdint.h>
#include "LCD.h"

//typedef char bool;
#define true 1
#define false 0

extern void xpt2046_init(void);
extern void xpt2046_read_xy(uint16_t *phwXpos, uint16_t *phwYpos);
extern uint8_t xpt2046_twice_read_xy(uint16_t *phwXpos, uint16_t *phwYpos);

#endif



