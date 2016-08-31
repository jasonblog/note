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
#include "genInstr.h"
#include "genVar.h"
#include "exception.h"
#include "global_var.h"
#include "genFunc.h"



/* Suffix	Name	      Size
 *   B	    BYTE    	1 byte (8 bits)
 *   W	    WORD    	2 bytes (16 bits)
 *   L	    LONG    	4 bytes (32 bits)
 *   Q	    QUADWORD	8 bytes (64 bits)
 */
static void instrAddSuffix(char *origin_instr, uint32_t byte_size)
{
	char *suffix;
	switch (byte_size) {
	/*JCC always use 8-bytes regs*/
	case BYTE_1:
	case BYTE_4:
	case BYTE_8:
		suffix = "q";
		break;
	default:
		error("Instruct size error!");
		break;
	}
	strcat(origin_instr, suffix);
}



/*user should not use this function directly,the macro is better*/
void instrMOV(uint32_t instrType, uint32_t byte_size, uint32_t value, char *reg_1, char *reg_2, uint32_t offset, char *sym_name)
{
	char instr[5] = "mov";
	instrAddSuffix(instr, byte_size);

	asmPrintf_func("    ");
	switch (instrType) {
	case VALUE_REG:
		asmPrintf_func("%s    $%d,%%%s\n", instr, value, reg_1);
		break;
	case REG_OFFSET:
		asmPrintf_func("%s    %%%s, %d(%%%s)\n", instr, reg_1, offset, reg_2);
		break;
	case REG_REG:
		asmPrintf_func("%s    %%%s, %%%s\n", instr, reg_1, reg_2);
		break;
	case VALUE_OFFSET:
		asmPrintf_func("%s    $%d, %d(%%%s)\n", instr, value, offset, reg_1);
		break;
	case OFFSET_REG:
		asmPrintf_func("%s    %d(%%%s), %%%s\n", instr, offset, reg_1, reg_2);
		break;
	case symOFFSET_REG:
		asmPrintf_func("%s    %s(%%%s), %%%s\n", instr, sym_name, reg_1, reg_2);
		break;
	case REG_symOFFSET:
		asmPrintf_func("%s    %%%s, %s(%%%s)\n", instr, reg_1, sym_name, reg_2);
		break;
	default:
		error("MOV instruction type error");
		break;
	}
}

static void genMUL_rax(uint32_t op, uint32_t mode)
{
	char instr[6];
	uint32_t reg;

	if (mode == RET_AT_TOP) {
		operand_pop();
	}

	switch (op) {
	case tk_STAR:
		strcpy(instr, "imulq");
		reg = FindFreeReg();
		assignReg(reg);
		asmPrintf_func("    %s   %%%s, %%%s\n", instr, reg_pool[reg].reg_name, reg_pool[REG_RAX].reg_name);/*store to second reg*/
		if (mode == RET_AT_SECOND) {
			operand_pop();
		}
		setReg_Return(REG_RAX);
		setReg_Unused(reg);
		break;
	case tk_DIVIDE:
	case tk_MOD:
		strcpy(instr, "idivq");
		FreeReg(REG_RCX);
		assignReg(REG_RCX);
		FreeReg(REG_RAX);
		assignReg(REG_RAX);
		asmPrintf_func("    cqo\n");/*negative idiv : http://stackoverflow.com/questions/10343155/x86-assembly-handling-the-idiv-instruction*/
		asmPrintf_func("    %s   %%%s\n", instr, reg_pool[REG_RCX].reg_name); /*quotient:rax   ,  remainder:rdx*/
		if (op == tk_MOD) {
			instrMOV_REG_REG(BYTE_8, reg_pool[REG_RDX].reg_name, reg_pool[REG_RAX].reg_name);
		}
		setReg_Return(REG_RAX);
		setReg_Unused(REG_RDX);
		setReg_Unused(REG_RCX);
		break;
	default:
		interERROR("MUL type instruction generating error!");
		break;
	}
	operand_push(&ret_sym, NOT_SPECIFIED);

}


void genMUL(uint32_t op)
{
	char instr[6];
	uint32_t reg;

	if (opTop->sym->storage_type == JC_RET_REG) {/*for some operand has higher priority than previous one*/
		genMUL_rax(op, RET_AT_TOP);
		return;
	} else if (opTop[-1].sym->storage_type == JC_RET_REG) { /*for several same priority operand_push*/
		genMUL_rax(op, RET_AT_SECOND);
		return;
	}

	switch (op) {
	case tk_STAR:
		strcpy(instr, "imulq");
		reg = FindFreeReg();
		assignReg_twoSecond(reg);
		assignReg_twoFirst(REG_RAX);
		asmPrintf_func("    %s   %%%s, %%%s\n", instr, reg_pool[reg].reg_name, reg_pool[REG_RAX].reg_name);/*store to second reg*/
		setReg_Return(REG_RAX);
		setReg_Unused(reg);
		operand_push(&ret_sym, NOT_SPECIFIED);
		break;
	case tk_DIVIDE:
	case tk_MOD:
		strcpy(instr, "idivq");
		assignReg_twoSecond(REG_RCX);/*reaminder*/
		assignReg_twoFirst(REG_RAX);/*quotient*/
		asmPrintf_func("    cqo\n");/*negative idiv : http://stackoverflow.com/questions/10343155/x86-assembly-handling-the-idiv-instruction*/
		asmPrintf_func("    %s   %%%s\n", instr, reg_pool[REG_RCX].reg_name); /*quotient:rax   ,  remainder:rdx*/
		if (op == tk_MOD) {
			instrMOV_REG_REG(BYTE_8, reg_pool[REG_RDX].reg_name, reg_pool[REG_RAX].reg_name);
		}
		setReg_Return(REG_RAX);
		setReg_Unused(REG_RDX);
		setReg_Unused(REG_RCX);
		operand_push(&ret_sym, NOT_SPECIFIED);
		break;
	default:
		interERROR("MUL type instruction generating error!");
		break;
	}
}


static void genADD_rax(uint32_t op, uint32_t mode)
{
	char instr[5];
	uint32_t reg;

	switch (op) {
	case tk_PLUS:
		strcpy(instr, "addq");
		break;
	case tk_MINUS:
		strcpy(instr, "subq");
		break;
	default:
		interERROR("ADD type instruction generating error!");
		break;
	}
	if (mode == RET_AT_TOP) {
		operand_pop();
	}
	reg = FindFreeReg();
	assignReg(reg);
	asmPrintf_func("    %s    %%%s, %%%s\n", instr, reg_pool[reg].reg_name, reg_pool[REG_RAX].reg_name);/*store to second reg*/

	if (mode == RET_AT_SECOND) {
		operand_pop();
	}

	setReg_Return(REG_RAX);
	setReg_Unused(reg);
	operand_push(&ret_sym, NOT_SPECIFIED);
}


void genADD(uint32_t op)
{
	char instr[5];
	uint32_t reg;

	if (opTop->sym->storage_type == JC_RET_REG) {  /*for some operand has higher priority than previous one*/
		genADD_rax(op, RET_AT_TOP);
		return;
	} else if (opTop[-1].sym->storage_type == JC_RET_REG) { /*for several same priority operand_push*/
		genADD_rax(op, RET_AT_SECOND);
		return;
	}

	switch (op) {
	case tk_PLUS:
		strcpy(instr, "addq");
		break;
	case tk_MINUS:
		strcpy(instr, "subq");
		break;
	default:
		interERROR("ADD type instruction generating error!");
		break;
	}
	reg = FindFreeReg();
	assignReg_twoSecond(reg);
	assignReg_twoFirst(REG_RAX);
	asmPrintf_func("    %s    %%%s, %%%s\n", instr, reg_pool[reg].reg_name, reg_pool[REG_RAX].reg_name);/*store to second reg*/
	setReg_Return(REG_RAX);
	setReg_Unused(reg);
	operand_push(&ret_sym, NOT_SPECIFIED);
}



/*always put first var at RSI,second var at RDI,please note the order of parameters for cmp instruction*/
void genCMP(uint32_t op)
{
	FreeReg(REG_RDI);
	assignReg(REG_RDI);
	FreeReg(REG_RSI);
	assignReg(REG_RSI);
	asmPrintf_func("    cmpq    %%rdi,%%rsi\n");
	FreeReg(REG_RDI);
	FreeReg(REG_RSI);

	operand_push(NULL, NOT_SPECIFIED);
	opTop->tk_code = op;
}

void genJMP_IF(void)
{
	uint32_t op = opTop->tk_code;
	char condition_code[4];
	operand_pop();
	switch (op) {
	case tk_EQ:
		strcpy(condition_code, "jne");
		break;
	case tk_NEQ:
		strcpy(condition_code, "je");
		break;
	case tk_LT:
		strcpy(condition_code, "jge");
		break;
	case tk_LEQ:
		strcpy(condition_code, "jg");
		break;
	case tk_GT:
		strcpy(condition_code, "jle");
		break;
	case tk_GEQ:
		strcpy(condition_code, "jl");
		break;
	default:
		interERROR("genJMP_IF with wrong op");
		break;
	}

	asmPrintf_func("    %s  .L_ELSE%d\n", condition_code, (opTop->tk_code & JC_IF_NESTED_MASK));
}


void genJMP_FOR(void)
{
	uint32_t op = opTop->tk_code;
	char condition_code[4];
	operand_pop();
	switch (op) {
	case tk_EQ:
		strcpy(condition_code, "jne");
		break;
	case tk_NEQ:
		strcpy(condition_code, "je");
		break;
	case tk_LT:
		strcpy(condition_code, "jge");
		break;
	case tk_LEQ:
		strcpy(condition_code, "jg");
		break;
	case tk_GT:
		strcpy(condition_code, "jle");
		break;
	case tk_GEQ:
		strcpy(condition_code, "jl");
		break;
	default:
		interERROR("genJMP_FOR with wrong op");
		break;
	}
	asmPrintf_func("    %s  .F_END_%d\n", condition_code, opTop->tk_code & JC_FOR_NESTED_MASK);
	operand_pop();
}


