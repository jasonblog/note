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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "declaration.h"
#include "global_var.h"
#include "token.h"
#include "exception.h"
#include "lex.h"
#include "statement.h"
#include "expression.h"
#include "reg.h"
#include "genFunc.h"
#include "operand.h"
#include "genInstr.h"
#include "auxFunc.h"

/********************************************
 * <statement>::=<compound_statement>
 *             | <if_statement>
 *             | <return_statement>
 *             | <break_statement>
 *             | <continue_statement>
 *             | <for_statement>
 *             | <expression_statement>
 ********************************************/
void statement(uint32_t *break_sym, uint32_t *continue_sym)
{
	switch (cur_token) {
	case tk_BEGIN:
		compound_statement(break_sym, continue_sym);
		break;
	case kw_IF:
		if_statement(break_sym, continue_sym);
		break;
	case kw_RETURN:
		return_statement();
		break;
	case kw_BREAK:
		break_statement();
		break;
	case kw_CONTINUE:
		continue_statement();
		break;
	case kw_FOR:
		for_statement(break_sym, continue_sym);
		break;
	default:
		expression_statement();
		break;
	}
}



/********************************************
 * <compound_statement>::=<tk_BEGIN>{<declaration>}{<statement>}<tk_END>
 ********************************************/
void compound_statement(uint32_t *break_sym, uint32_t *continue_sym)
{
	Symbol *s;
	s = (Symbol*)stack_get_top(&local_sym_stack);

	/*JCC only support declarae local variable before any statement*/
	getToken();
	while (is_type_specifier(cur_token)) {
		external_declaration(JC_LOCAL);
	}


	while (cur_token != tk_END) {
		statement(break_sym, continue_sym);
	}
	sym_pop(&local_sym_stack, s);
	getToken();
}


/********************************************
 * To knopw whether it is type specifier
 ********************************************/
uint32_t is_type_specifier(uint32_t tk_code)
{
	switch (tk_code) {
	case kw_CHAR:
	case kw_INT:
	case kw_VOID:
	case kw_STRUCT:
		return 1;
	default:
		break;
	}
	return 0;
}


/********************************************
 * <expression_statement>::=<tk_SEMICOLON>
 ********************************************/
void expression_statement(void)
{
	if (cur_token != tk_SEMICOLON) {
		expression();
	}
	skip(tk_SEMICOLON);
}



/********************************************
 * <if_statement>::=<kw_IF><tk_openPA><expression><tk_closePA><statement>[<kw_ELSE><statement>]
 ********************************************/
void if_statement(uint32_t *break_sym, uint32_t *continue_sym)
{
	uint32_t after_if = JC_IF | condtion_label_count;
	uint32_t fix_label;
	operand_push(NULL, NOT_SPECIFIED);
	opTop->tk_code = after_if;
	condtion_label_count++;

	getToken();
	skip(tk_openPA);
	expression();
	skip(tk_closePA);
	genJMP_IF();
	statement(break_sym, continue_sym);
	if (cur_token == kw_ELSE) {
		operand_push(NULL, NOT_SPECIFIED);
		opTop->tk_code = JC_IF | condtion_label_count;
		condtion_label_count++;

		asmPrintf_func("    jmp  .L%d\n", after_if & JC_IF_NESTED_MASK);
	}
	if (opTop->tk_code & JC_IF) {
		fix_label = opTop->tk_code & JC_IF_NESTED_MASK;
		asmPrintf_func(".L%d:\n", opTop->tk_code & JC_IF_NESTED_MASK);
		operand_pop();
	}

	/*prepare for replace previous label*/
	char target_original[13];
	char target_new[13];
	sprintf(target_original, "%s%d", ".L_ELSE", after_if & JC_IF_NESTED_MASK);

	if (cur_token == kw_ELSE) {
		getToken();
		statement(break_sym, continue_sym);

		/*replace previous ".L_ELSE+(after_if)" with ".L(fix_label)" */
		sprintf(target_new, ".L%d\n", fix_label);
		replace_output(target_original, target_new);

		asmPrintf_func(".L%d:\n", (opTop->tk_code & JC_IF_NESTED_MASK));
		operand_pop();
	} else {
		/*replace ".L_ELSE+(after_if)" with ".L(after_if)"*/
		sprintf(target_new, ".L%d\n", after_if & JC_IF_NESTED_MASK);
		replace_output(target_original, target_new);
	}
}


/********************************************
 * <for_statement>::=
 *               <kw_FOR><tk_openPA><expression_statement><expression_statement><expression><tk_closePA><statement>
 *******************************************/
void for_statement(uint32_t *break_sym, uint32_t *continue_sym)
{
	uint32_t for_label;

	getToken();
	skip(tk_openPA);
	if (cur_token != tk_SEMICOLON) {
		expression();
	}
	skip(tk_SEMICOLON);

	for_label_count++;
	asmPrintf_func(".F%d:\n", for_label_count);
	operand_push(NULL, NOT_SPECIFIED);
	for_label = JC_FOR | for_label_count;
	opTop->tk_code = for_label;

	if (cur_token != tk_SEMICOLON) {
		expression();
	}
	skip(tk_SEMICOLON);
	genJMP_FOR();

	if (cur_token != tk_closePA) {
		expression();
	}

	skip(tk_closePA);
	statement(break_sym, continue_sym);
	asmPrintf_func("    jmp  .F%d\n", for_label & JC_FOR_NESTED_MASK);
	asmPrintf_func("\n.F_END_%d:\n", for_label & JC_FOR_NESTED_MASK);
}


/*********************************************
 * <continue_statement>::=<kw_CONTINUE><tk_SEMICOLON>
 *********************************************/
void continue_statement(void)
{
	getToken();
	skip(tk_SEMICOLON);
}


/*********************************************
 * <break_statement>::=<kw_BREAK><tk_SEMICOLON>
 *********************************************/
void break_statement(void)
{
	getToken();
	skip(tk_SEMICOLON);
}


/*********************************************
 * <return_statement>::=<kw_RETURN><tk_SEMICOLON>
 *                    | <kw_RETURN><expression><tk_SEMICOLON>
 *********************************************/
void return_statement(void)
{
	getToken();

	if (cur_token != tk_SEMICOLON) {
		expression();
		/*for doing expression that has return value,it should be stored in RAX*/
		if (opTop->sym->storage_type & JC_RET_REG) {
			/*do nothing*/
		} else if (opTop->sym->storage_type & JC_LOCAL) { /*for local variable*/
			instrMOV_OFFSET_REG(BYTE_8, "rbp", reg_pool[REG_RAX].reg_name, opTop->sym->fp_offset);
		} else if (opTop->sym->storage_type & JC_CONST) { /*for const value*/
			instrMOV_VAL_REG(BYTE_8, opTop->value, reg_pool[REG_RAX].reg_name);
		} else if (opTop->sym->storage_type & JC_GLOBAL) { /*for global variable*/
			instrMOV_symOFFSET_REG(BYTE_8, get_tkSTR(opTop->sym->tk_code), "rip", reg_pool[REG_RAX].reg_name);
		} else {
			error("return ERROR");
		}
		operand_pop();
	}
	skip(tk_SEMICOLON);
}

















