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

#include <string.h>
#include "reg.h"
#include "symbol.h"
#include "token.h"
#include "global_var.h"
#include "operand.h"
#include "genInstr.h"
#include "exception.h"
#include "genFunc.h"


void RegPoolInit(void)
{
	/*reg_pool[0] should not use*/
	for (int i = REG_BASE + 1; i < REG_MAX; i++) {
		reg_pool[i].owner = NULL;
		reg_pool[i].usage = REG_NOT_USING;
	}
	strcpy(reg_pool[REG_RDI].reg_name, "rdi");
	strcpy(reg_pool[REG_RSI].reg_name, "rsi");
	strcpy(reg_pool[REG_RDX].reg_name, "rdx");
	strcpy(reg_pool[REG_RCX].reg_name, "rcx");
	strcpy(reg_pool[REG_RAX].reg_name, "rax");
	strcpy(reg_pool[REG_R8].reg_name, "r8");
	strcpy(reg_pool[REG_R9].reg_name, "r9");
}


uint32_t FindFreeReg(void)
{
	for (int i = REG_BASE + 1; i < REG_MAX; i++) {
		if (reg_pool[i].usage == REG_NOT_USING) {
			return i;
		}
	}
	for (int i = REG_BASE + 1; i < REG_MAX; i++) {
		/*free the reg if it will not be used in the next step*/
		if (reg_pool[i].usage & ~REG_WILL_USE) {
			FreeReg(i);
			return i;
		}
	}
	interERROR("Finding free reg error!");
	return 0;
}


void FreeReg(uint32_t REGx)
{
	REG *target = &reg_pool[REGx];
	if (reg_pool[REGx].usage & REG_IS_USING) { /*Free REG_IS_USING*/
		if (target->owner->storage_type & JC_CONST) { /*constant value in global*/
			/*const value will not be modified*/
			target->usage = REG_NOT_USING;
			return;
		} else if (target->owner->storage_type & JC_LOCAL & JC_LVAL) { /*local variable*/
			char reg_2[] = "rbp";
			target->usage = REG_NOT_USING;
			instrMOV_REG_OFFSET(8, target->reg_name, reg_2, target->owner->fp_offset);
		} else if (target->owner->storage_type & JC_GLOBAL & JC_LVAL) { /*global variable*/
			char reg_2[] = "rip";
			target->usage = REG_NOT_USING;
			instrMOV_REG_symOFFSET(8, get_tkSTR(target->owner->tk_code & JC_ValMASK), target->reg_name, reg_2);
		}
	} else if (reg_pool[REGx].usage == REG_RETURN_VAL) {
		target->usage = REG_NOT_USING;
	} else if (reg_pool[REGx].usage == REG_NOT_USING) {
		/*do nothing*/
	} else {
		interERROR("Freeing the reg with REG_WILL_USE");
	}
}

void FreeAllReg(void)
{
	for (int i = REG_BASE + 1; i < REG_MAX; i++) {
		FreeReg(i);
	}
}

void assignReg(uint32_t REGx)
{
	REG *target = &reg_pool[REGx];
	char src_reg[4];
	if (target->usage == REG_NOT_USING) {
		/*do nothing*/
	} else if (target->usage == REG_IS_USING) {
		FreeReg(REGx);
	} else {
		interERROR("reg usage error in Assigning!");
	}
	target->usage = REG_IS_USING;
	target->owner = opTop->sym;
	target->value = opTop->value;
	operand_pop();

	if (opTop->tk_code == JC_getREFERENCE) {
		if ((target->owner->storage_type & JC_GLOBAL) && !(target->owner->type.data_type & T_PTR)) {
			asmPrintf_func("    movq    $%s, %%%s\n", get_tkSTR(target->owner->tk_code), reg_pool[REGx].reg_name);
		} else if ((target->owner->storage_type & (JC_LOCAL)) && (target->owner->storage_type & (JC_LVAL))) {
			asmPrintf_func("    leaq    %d(%%%s), %%%s\n", target->owner->fp_offset, "rbp", reg_pool[REGx].reg_name);
		} else {
			error("JCC's get reference is limitted");
		}
		operand_pop();
		return;
	}

	if ((target->owner->storage_type & JC_GLOBAL) && !(target->owner->type.data_type & T_PTR)) {
		if (target->owner->storage_type & JC_LVAL) {
			strcpy(src_reg, "rip");
			instrMOV_symOFFSET_REG(8, get_tkSTR(target->owner->tk_code), src_reg, reg_pool[REGx].reg_name);
		}
		if (target->owner->storage_type & JC_CONST) {
			instrMOV_VAL_REG(8, target->value, reg_pool[REGx].reg_name);
		}
	} else if ((target->owner->storage_type & (JC_LOCAL)) && (target->owner->storage_type & (JC_LVAL))) {
		strcpy(src_reg, "rbp");
		instrMOV_OFFSET_REG(8, src_reg, reg_pool[REGx].reg_name, target->owner->fp_offset);
	} else {
		if (target->owner->tk_code != 0x0) {
			strcpy(src_reg, "rip");
			instrMOV_symOFFSET_REG(8, get_tkSTR(target->owner->tk_code), src_reg, reg_pool[REGx].reg_name);
		} else {
			/*const str,not variable*/
			asmPrintf_func("    movq    $.LC%d, %%%s\n", target->owner->fp_offset, reg_pool[REGx].reg_name);
		}
	}
}


void assignReg_twoFirst(uint32_t REGx)
{
	assignReg(REGx);/*always restore the target reg and assign opStack top->sym to it,then pop*/
	reg_pool[REGx].usage |= REG_WILL_USE;
}

void assignReg_twoSecond(uint32_t REGx)
{
	assignReg(REGx);
}


void setReg_Unused(uint32_t REGx)
{
	reg_pool[REGx].usage = REG_NOT_USING;
}


void setReg_Return(uint32_t REGx)
{
	reg_pool[REGx].usage = REG_RETURN_VAL;
}
