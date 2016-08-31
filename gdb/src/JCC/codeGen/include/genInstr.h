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
#ifndef __GEN_INSTR_H_
#define __GEN_INSTR_H_

#include "global_var.h"
#include "stdint.h"
#include "symbol.h"

/*instruction type*/
#define VALUE_REG     0x0
#define REG_OFFSET    0x1
#define REG_REG       0x2
#define VALUE_OFFSET  0x3
#define OFFSET_REG    0x4
#define symOFFSET_REG 0x5
#define REG_symOFFSET 0x6

/*Instruction postfix size*/
#define BYTE_1    0x1
#define BYTE_4    0x4
#define BYTE_8    0x8


#define instrMOV_VAL_REG(byte_size,value,reg)          instrMOV(VALUE_REG, byte_size, value, reg, NULL, NOT_SPECIFIED, NULL)
#define instrMOV_REG_OFFSET(byte_size,reg_1,reg_2,offset_2)     instrMOV(REG_OFFSET, byte_size, NOT_SPECIFIED, reg_1, reg_2, offset_2, NULL)
#define instrMOV_REG_REG(byte_size, reg_1, reg_2)      instrMOV(REG_REG, byte_size, NOT_SPECIFIED, reg_1, reg_2, NOT_SPECIFIED, NULL)
#define instrMOV_VAL_OFFSET(byte_size, value, reg_1, offset_2)  instrMOV(VALUE_OFFSET, byte_size, value, reg_1, NULL, offset_2,NULL)
#define instrMOV_OFFSET_REG(byte_size, reg_1, reg_2, offset_1)  instrMOV(OFFSET_REG, byte_size, NOT_SPECIFIED, reg_1, reg_2, offset_1,NULL)
#define instrMOV_symOFFSET_REG(byte_size, sym_name, reg_1, reg_2)  instrMOV(symOFFSET_REG, byte_size, NOT_SPECIFIED, reg_1, reg_2, NOT_SPECIFIED,sym_name)
#define instrMOV_REG_symOFFSET(byte_size, sym_name, reg_1, reg_2)  instrMOV(REG_symOFFSET, byte_size, NOT_SPECIFIED, reg_1, reg_2, NOT_SPECIFIED,sym_name)
void instrMOV(uint32_t instrType, uint32_t byte_size, uint32_t value, char *reg_1, char *reg_2, uint32_t offset, char *sym_name);


#define RET_AT_TOP     0x1
#define RET_AT_SECOND  0x2
void genMUL(uint32_t op);
void genADD(uint32_t op);
void genCMP(uint32_t op);
void genJMP_IF(void);
void genJMP_FOR(void);

#endif

