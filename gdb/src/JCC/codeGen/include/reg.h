/*
 * Copyright (c) 2016, Chang Jia-Rung, All Rights Reserved
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __REG_H
#define __REG_H

#include "operand.h"

/*define usable reg*/
#define  REG_BASE      0x0
#define  REG_RDI       0x1
#define  REG_RSI       0x2
#define  REG_RDX       0x3
#define  REG_RCX       0x4
#define  REG_RAX       0x5
#define  REG_R8        0x6
#define  REG_R9        0x7
#define  REG_MAX       REG_R9+1

#define  REG_NOT_USING   0xF000
#define  REG_IS_USING    0x0F00
#define  REG_WILL_USE    0x00F0/*cannot be found by FindFreeREG*/
#define  REG_RETURN_VAL  0x000F


typedef struct {
	Symbol *owner;
	int value;/*for const int/char */
	char reg_name[4];
	uint32_t usage;
} REG;

void RegPoolInit(void);
uint32_t FindFreeReg(void);
void FreeReg(uint32_t REGx);
void FreeAllReg(void);
void assignReg(uint32_t REGx);
void assignReg_twoFirst(uint32_t REGx);
void assignReg_twoSecond(uint32_t REGx);
void setReg_Unused(uint32_t REGx);
void setReg_Return(uint32_t REGx);



#endif

