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
#include "expression.h"
#include "declaration.h"
#include "global_var.h"
#include "token.h"
#include "exception.h"
#include "lex.h"
#include "statement.h"
#include "genFunc.h"
#include "genInstr.h"
#include "operand.h"
#include "genVar.h"

/**************************************************
 * <expression>::=<assignment_expression>{<tk_COMMA><assignment_expression>}
 **************************************************/
void expression(void)
{
	while (1) {
		assignment_expression();
		if (cur_token != tk_COMMA) {
			break;
		}
		getToken();
	}
}


static void check_LValue(void)
{
	if (!(opTop->sym->storage_type & JC_LVAL)) {
		expect("LValue");
	}
}


/**************************************************
 * <assignment_expression>::=<equality_expression>
 *                         | <unary_expression><tk_ASSIGN><assignment_expression>
 *
 * We modified the grammar into:
 *
 * <assignment_expression>::=<equality_expression>{<tk_ASSIGN><assignment_expression>}
 **************************************************/
void assignment_expression(void)
{
	equality_expression();
	if (cur_token == tk_ASSIGN) {
		check_LValue();
		getToken();
		assignment_expression();
		genAssign();
	}
}


/**************************************************
 * <equality_expression>::=<relational_expression>
 *                       { <tk_EQ><relational_expression>
 *                       | <tk_NEQ><relational_expression>
 *                       }
 **************************************************/
void equality_expression(void)
{
	uint32_t tk_code;
	relational_expression();
	while ((cur_token == tk_EQ) || (cur_token == tk_NEQ)) {
		tk_code = cur_token;
		getToken();
		relational_expression();
		genCMP(tk_code);
	}
}


/**************************************************
 * <relational_expression>::=<additive_expression>
 *                        {
 *                           <tk_LT><additive_expression>
 *                         | <tk_GT><additive_expression>
 *                         | <tk_LEQ><additive_expression>
 *                         | <tk_GEQ><additive_expression>
 *                        }
 **************************************************/
void relational_expression(void)
{
	uint32_t tk_code;
	additive_expression();
	while ((cur_token == tk_LT) || (cur_token == tk_LEQ) || (cur_token == tk_GT) || (cur_token == tk_GEQ)) {
		tk_code = cur_token;
		getToken();
		additive_expression();
		genCMP(tk_code);
	}
}


/**************************************************
 * <additive_expression>::=<multiplicative_expression>
 *                      {
 *                        <tk_PLUS><multiplicative_expression>
 *                      | <tk_MINUS><multiplicative_expression>
 *                      }
 **************************************************/
void additive_expression(void)
{
	uint32_t tk;
	multiplicative_expression();
	/*JCC does not support minus a constant value."ex:i = i -1" */
	while ((cur_token == tk_PLUS) || (cur_token == tk_MINUS)) {
		tk = cur_token;
		getToken();
		multiplicative_expression();
		genADD(tk);
	}
}


/**************************************************
 * <multiplicative_expression>::=<unary_expression>
 *                             {
 *                               <tk_STAR><unary_expression>
 *                             | <tk_DIVIDE><unary_expression>
 *                             | <tk_MOD><unary_expression>
 *                             }
 **************************************************/
void multiplicative_expression(void)
{
	uint32_t tk;
	unary_expression();
	while ((cur_token == tk_STAR) || (cur_token == tk_DIVIDE) || (cur_token == tk_MOD)) {
		tk = cur_token;
		getToken();
		unary_expression();
		genMUL(tk);
	}
}


/**************************************************
 * <unary_expression>::=<postfix_expression>
 *                    | <tk_AND><unary_expression>
 *                    | <tk_STAR><unary_expression>
 *                    | <tk_PLUS><unary_expression>
 *                    | <tk_MINUS><unary_expression>
 *                    | <sizeof_expression>
 **************************************************/
void unary_expression(void)
{
	switch (cur_token) {
	case tk_AND:
		operand_push(NULL, NOT_SPECIFIED);
		opTop->tk_code = JC_getREFERENCE;
		getToken();
		unary_expression();
		break;
	case tk_STAR:
		getToken();
		unary_expression();
		break;
	case tk_PLUS:
		getToken();
		unary_expression();
		break;
	case tk_MINUS:
		getToken();
		unary_expression();
		break;
	case kw_SIZEOF:
		sizeof_expression();
		break;
	default:
		postfix_expression();
		break;
	}
}


/**************************************************
 * <sizeof_expression>::=<kw_SIZEOF><tk_openPA><type_specifier><tk_closePA>
 **************************************************/
void sizeof_expression(void)
{
	uint32_t align, size;
	Type type;

	getToken();
	skip(tk_openPA);
	type_specifier(&type);
	skip(tk_closePA);

	size = type_size(&type, &align);
	if (size < 0) {
		error("sizeof calcuation failed!");
	}
}


/**************************************************
 * <postfix_expression>::=<primary_expression>
 *                     {
 *                        <tk_openBR><expression><tk_closeBR>
 *                      | <tk_openPA><tk_closePA>
 *                      | <tk_openPA><argument_expression_list><tk_closePA>
 *                      | <tk_DOT><IDENTIFIER>
 *                      | <tk_POINTTO><IDENTIFIER>
 *                     }
 **************************************************/
void postfix_expression(void)
{
	primary_expression();
	while (1) {
		if ((cur_token == tk_DOT) || (cur_token == tk_POINTTO)) {
			getToken();
			cur_token |= JC_MEMBER;
			getToken();
		} else if (cur_token == tk_openBR) {
			getToken();
			expression();
			skip(tk_closeBR);
		} else if (cur_token == tk_openPA) {
			argument_expression_list();
		} else {
			break;
		}
	}
}


/**************************************************
 * <primary_expression>::=<IDENTIFIER>
 *                      | <tk_cINT>
 *                      | <tk_cSTR>
 *                      | <tk_cCHAR>
 *                      | <tk_openPA><expression><tk_closePA>
 **************************************************/
void primary_expression(void)
{
	uint32_t tk_expression;
	Type type;
	Symbol *ss;

	switch (cur_token) {
	case tk_cINT:
		operand_push(&int_sym, tkValue);
		getToken();
		break;
	case tk_cCHAR:
		operand_push(&char_sym, tkValue);
		getToken();
		break;
	case tk_cSTR:
		type.data_type = T_CHAR;
		mk_pointer(&type);
		type.data_type |= T_ARRAY;
		ss = var_sym_put(&type, JC_GLOBAL | JC_CONST, NOT_SPECIFIED, NOT_SPECIFIED);
		genGlobalVar(ss);
		operand_push(ss, NOT_SPECIFIED);
		initializer(&type);
		break;
	case tk_openPA:
		getToken();
		expression();
		skip(tk_closePA);
		break;
	case kw_PRINTF:
	case kw_SCANF:
	default:
		tk_expression = cur_token;
		getToken();
		if ((tk_expression < tk_IDENT) && (tk_expression != kw_PRINTF) && (tk_expression != kw_SCANF)) {
			expect("Identifier or constant value(char/string/number)");
		}
		ss = sym_search(tk_expression);
		if (!ss) {
			if (cur_token != tk_openPA) {
				error("'%s' is undeclared!", get_tkSTR(tk_expression));
			}
			ss = func_sym_push(tk_expression, &default_func_type); /*Allowing function can be invoked without declaration*/
			ss->storage_type = JC_GLOBAL | JC_SYM;
		}
		operand_push(ss, NOT_SPECIFIED);
		break;
	}
}


/**************************************************
 * <argument_expression_list>::=<assignment_expression>{<tk_COMMA><assignment_expression>}
 **************************************************/
void argument_expression_list(void)
{
	Symbol *func_definition_sym;/*mother/base symbol*/
	Symbol *func_para_sym;/*parameter symbol*/
	func_definition_sym = opTop->sym->type.ref;
	func_para_sym = func_definition_sym->next;
	/*if JCC need to check return type in future,we can use the func_definition_sym->type.data_type*/

	uint32_t args_num = 0;

	getToken();
	if (cur_token != tk_closePA) {
		while (1) {
			assignment_expression();
			args_num++;

			/*if there are next parameters in the function definition */
			if (func_para_sym) {
				func_para_sym = func_para_sym->next;
			}

			if (cur_token == tk_closePA) {
				break;
			}
			skip(tk_COMMA);
		}
	}

	if (func_para_sym) {
		error("actual parameters less than the function definition requests");
	}

	skip(tk_closePA);
	/*generating function call asm*/
	genFuncCall(args_num);
	if (((func_definition_sym->type.data_type & T_BTYPE) == T_INT) || ((func_definition_sym->type.data_type & T_BTYPE) == T_CHAR)) {
		operand_push(NULL, NOT_SPECIFIED);
		opTop->data_type.data_type = JC_FUNC_RET;
	}
}













