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
#include <stdlib.h>
#include "genFunc.h"
#include "genVar.h"
#include "token.h"
#include "exception.h"
#include "symbol.h"
#include "genInstr.h"
#include "global_var.h"

void genFileTitle(void)
{
	asmPrintf(".file   \"%s\"\n\n\n", cur_filename);
}

static uint32_t calc_func_argc(Symbol *sym)
{
	uint32_t count = 0;
	Symbol *sym_para = sym->type.ref;
	while (sym_para->next != NULL) {
		sym_para = sym_para->next;
		count++;
	}
	return count;
}

void genFuncProlog(Symbol *sym)
{
	uint32_t func_argc;
	uint32_t size = BYTE_8;
	Symbol *sym_para;
	Symbol *var_sym;
	char src_reg[4], dest_reg[4];
	char *func_name = get_tkSTR(sym->tk_code & JC_ValMASK);
	asmPrintf_func("\n    .text\n");
	asmPrintf_func("    .globl  %s\n", func_name);
	asmPrintf_func("    .type   %s, @function\n", func_name);
	asmPrintf_func("%s:\n", func_name);
	asmPrintf_func("    pushq   %%rbp\n");
	asmPrintf_func("    movq    %%rsp, %%rbp\n\n");

	strcpy(dest_reg, "rbp");
	func_argc = calc_func_argc(sym);
	for (int i = 0; i < func_argc; i++) {
		switch (i) {
		case 0:
			strcpy(src_reg, reg_pool[REG_RDI].reg_name);
			sym_para = sym->type.ref->next;
			var_sym = var_sym_put(&(sym_para->type), JC_LOCAL | JC_LVAL, (sym_para->tk_code & JC_TK_MASK), NOT_SPECIFIED);
			asmPrintf_func("    subq    $%d, %%rsp\n", size);
			updateSYM_FPoffset(var_sym, size);
			instrMOV_REG_OFFSET(BYTE_8, src_reg, dest_reg, var_sym->fp_offset);
			break;
		case 1:
			strcpy(src_reg, reg_pool[REG_RSI].reg_name);
			sym_para = sym->type.ref->next->next;
			var_sym = var_sym_put(&(sym_para->type), JC_LOCAL | JC_LVAL, (sym_para->tk_code & JC_TK_MASK), NOT_SPECIFIED);
			asmPrintf_func("    subq    $%d, %%rsp\n", size);
			updateSYM_FPoffset(var_sym, size);
			instrMOV_REG_OFFSET(BYTE_8, src_reg, dest_reg, var_sym->fp_offset);
			break;
		case 2:
			strcpy(src_reg, reg_pool[REG_RDX].reg_name);
			sym_para = sym->type.ref->next->next->next;
			var_sym = var_sym_put(&(sym_para->type), JC_LOCAL | JC_LVAL, (sym_para->tk_code & JC_TK_MASK), NOT_SPECIFIED);
			asmPrintf_func("    subq    $%d, %%rsp\n", size);
			updateSYM_FPoffset(var_sym, size);
			instrMOV_REG_OFFSET(BYTE_8, src_reg, dest_reg, var_sym->fp_offset);
			break;
		case 3:
			strcpy(src_reg, reg_pool[REG_RCX].reg_name);
			sym_para = sym->type.ref->next->next->next->next;
			var_sym = var_sym_put(&(sym_para->type), JC_LOCAL | JC_LVAL, (sym_para->tk_code & JC_TK_MASK), NOT_SPECIFIED);
			asmPrintf_func("    subq    $%d, %%rsp\n", size);
			updateSYM_FPoffset(var_sym, size);
			instrMOV_REG_OFFSET(BYTE_8, src_reg, dest_reg, var_sym->fp_offset);
			break;
		default:
			interERROR("JCC only support at most 4 args");
			break;
		}
	}
}


void genFuncEpilog(Symbol *sym)
{
	char *func_name = get_tkSTR(sym->tk_code & JC_ValMASK);
	if (strcmp("main", func_name) == 0) { /*if this is main function*/
		asmPrintf_func("\n    leave\n");
	} else {
		instrMOV_REG_REG(BYTE_8, "rbp", "rsp");
		asmPrintf_func("    popq    %%rbp\n");
	}
	asmPrintf_func("    ret\n");
	asmPrintf_func("    .size   %s, .-%s\n\n", func_name, func_name);
}


void genFuncCall(uint32_t argc)
{
	char dest_reg[4];
	FreeAllReg();
	for (int i = argc; i > 0; i--) {
		switch (i) {
		case 4:/*arg 4*/
			FreeReg(REG_RCX);
			strcpy(dest_reg, reg_pool[REG_RCX].reg_name);
			assignReg(REG_RCX);
			reg_pool[REG_RCX].usage |= REG_WILL_USE;
			break;
		case 3:/*arg 3*/
			FreeReg(REG_RDX);
			strcpy(dest_reg, reg_pool[REG_RDX].reg_name);
			assignReg(REG_RDX);
			reg_pool[REG_RDX].usage |= REG_WILL_USE;
			break;
		case 2:/*arg 2*/
			FreeReg(REG_RSI);
			strcpy(dest_reg, reg_pool[REG_RSI].reg_name);
			assignReg(REG_RSI);
			reg_pool[REG_RSI].usage |= REG_WILL_USE;
			break;
		case 1:/*arg 1*/
			FreeReg(REG_RDI);
			strcpy(dest_reg, reg_pool[REG_RDI].reg_name);
			assignReg(REG_RDI);
			reg_pool[REG_RDI].usage |= REG_WILL_USE;
			break;
		default:
			error("JCC only suppory at most 4 arguments!");
			break;
		}
	}

	/*printf and scanf does not need function defintion,so we need to free the reg status manually*/
	if ((opTop->sym->tk_code == kw_PRINTF) || (opTop->sym->tk_code == kw_SCANF)) {
		for (int i = argc; i > 0; i--) {
			switch (i) {
			case 4:/*arg 4*/
				setReg_Unused(REG_RCX);
				break;
			case 3:/*arg 3*/
				setReg_Unused(REG_RDX);
				break;
			case 2:/*arg 2*/
				setReg_Unused(REG_RSI);
				break;
			case 1:/*arg 1*/
				setReg_Unused(REG_RDI);
				break;
			}
		}
		instrMOV_VAL_REG(BYTE_8, 0, reg_pool[REG_RAX].reg_name);
	}

	if (opTop->sym->tk_code == kw_SCANF) {
		FreeReg(REG_RAX);
		asmPrintf_func("    call    %s\n\n", "__isoc99_scanf");
		operand_pop();
	} else {
		FreeReg(REG_RAX);
		asmPrintf_func("    call    %s\n\n", get_tkSTR(opTop->sym->tk_code));
		operand_pop();
	}
}

