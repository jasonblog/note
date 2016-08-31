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
#include "declaration.h"
#include "statement.h"
#include "global_var.h"
#include "token.h"
#include "exception.h"
#include "lex.h"
#include "expression.h"
#include "symbol.h"
#include "genVar.h"
#include "genFunc.h"

/***************************************
 *<translation_unit>::={<external_declaration>}<tk_EOF>
 ***************************************/
void translation_unit(void)
{
	while (cur_token != tk_EOF) {
		external_declaration(JC_GLOBAL);
	}
}


/***************************************
 * <external_declaration>::=<function_definition> | <declarator>
 * <function_definition>::=<type_specifier><declarator><funcbody>
 * <declarator>::=<type_specifier><tk_SEMICOLON> | <type_specifier><init_declarator_list><tk_SEMICOLON>
 * <init_declarator_list>::=<init_declarator>{<tk_COMMA><init_declarator>}
 * <init_declarator>::=<declarator>{<tk_ASSIGN><initializer>}
 *
 * Above are equivalent to:
 *
 * <external_declaration>::=<type_specifier>(
 * <tk_SEMICOLON> |
 * <declarator><funcbody> |
 * <declarator>[<tk_ASSIGN><initializer>]
 * {<tk_COMMA><declarator>[<tk_ASSIGN><initializer>]}<tk_SEMICOLON>
 * )
 ***************************************/
/*external_definition,"external" does not mean global scope
 * EX:
 * int a;                        V
 * int add(int x,int y)          V
 * {
 *     int z;                    V
 *     z = x+y;
 * }
 *
 * with the right "V",that line is "external_definition"
 * */
void external_declaration(uint32_t storage_type)
{
	Type base_type, type;
	uint32_t tk, storage_type_1;
	Symbol *sym;

	/*struct definition will be dealed here,the code below does not deal with struct definition*/
	if (!type_specifier(&base_type)) {
		expect("<type_specifier>");
	}

	if ((base_type.data_type == T_STRUCT) && (cur_token == tk_SEMICOLON)) {
		getToken();
		return;
	}

	/* function definition or variable declaration*/
	while (1) {
		type = base_type;
		/* If encounter a variable/function declaration,cur_token is the token after the identifier.
		 *
		 * If this is the second loop of the function definition,declarator() will do nothing,
		 * then push the "function name symbol" into global stack.
		 */
		declarator(&type, &tk, NULL);

		/*If we encountered a function declaration,and now the cur_token is "{" which is ready to define the function */
		if (cur_token == tk_BEGIN) {
			if (storage_type == JC_LOCAL) {
				error("JCC does not support nested function definition!");
			}

			if ((type.data_type & T_BTYPE) != T_FUNC) {
				expect("<FUNCTION DEFINITION>");
			}

			sym = sym_search(tk);
			if (sym) { /*if function is already declared,give function definition*/
				if ((sym->type.data_type & T_BTYPE) != (T_FUNC)) {
					error("'%s' redefinition!", get_tkSTR(tk));
				}
				sym->type = type;
			} else {
				sym = func_sym_push(tk, &type);
			}
			sym->storage_type = JC_SYM | JC_GLOBAL;
			funcbody(sym);
			break;
		} else {
			if ((type.data_type & T_BTYPE) == T_FUNC) { /*function declaration*/
				if (sym_search(tk) == NULL) {
					sym = sym_push(tk, &type, JC_GLOBAL | JC_SYM, NOT_SPECIFIED);
				}
			} else { /*variable/pointer declaration*/
				storage_type_1 = NOT_SPECIFIED;
				if (!(type.data_type & T_ARRAY)) {
					storage_type_1 |= JC_LVAL;
				}
				storage_type_1 |= storage_type;

				tkValue = NOT_SPECIFIED;
				if (cur_token == tk_ASSIGN) {
					operand_push(NULL, NOT_SPECIFIED);
					opTop->tk_code = JC_VAR_INIT;
					getToken();
					initializer(&type);
					/* In variable declaration,JCC does not need the corresponding operand,but so pop it
					 * The operand_push in primary_expression() is for statement()
					 */
					/*pop JC_VAR_INIT*/
					operand_pop();
					/*pop tkValue operand*/
					operand_pop();
				}
				sym = var_sym_put(&type, storage_type_1, tk, tkValue);
				operand_push(sym, tkValue);
				genVar();
			}

			if (cur_token == tk_COMMA) {
				getToken();
			} else {
				skip(tk_SEMICOLON);
				break;
			}
		}
	}
}

/***************************************
 * <type_specifier>::=<kw_INT>   |
 *                    <kw_CHAR>  |
 *                    <kw_VOID>  |
 *                    <struct_specifier>
 ***************************************/
int type_specifier(Type *type)
{
	uint32_t type_found = 0;
	switch (cur_token) {
	case kw_CHAR:
		type->data_type = T_CHAR;
		type_found = 1;
		getToken();
		break;
	case kw_VOID:
		type->data_type = T_VOID;
		type_found = 1;
		getToken();
		break;
	case kw_INT:
		type->data_type = T_INT;
		type_found = 1;
		getToken();
		break;
	case kw_STRUCT:
		struct_specifier(type);
		type->data_type = T_STRUCT;
		type_found = 1;
		break;
	default:
		break;
	}
	return type_found;
}


/***************************************
 * <struct_specifier>::=<kw_STRUCT><IDENTIFIER><tk_BEGIN><struct_declaration_list><tk_END>
 *                    | <kw_STRUCT><IDENTIFIER>
 ***************************************/
void struct_specifier(Type *type)
{
	Symbol *s;
	Type type_1;
	uint32_t tk;

	getToken();
	tk = cur_token;
	getToken();

	if (tk < tk_IDENT) {
		expect("struct name!");
	}

	s = struct_search(tk);
	/*if struct is not defined yet*/
	if (!s) {
		type_1.data_type = kw_STRUCT;
		s = sym_push(tk | JC_STRUCT, &type_1, NOT_SPECIFIED, STRUCT_NOT_DEFINED);
	}

	type->data_type = T_STRUCT;
	type->ref = s;

	if (cur_token == tk_BEGIN) {
		struct_declaration_list(type);
	}
}


/* Ex:
 * calc_align(9,1) = 9
 * calc_align(9,2) = 10
 * calc_align(9,4) = 12
 */
static uint32_t calc_align(uint32_t size, uint32_t align)
{
	return ((size + align - 1) & (~(align - 1)));
}


/****************************************
 * <struct_declaration_list>::=<struct_declaration>{<struct_declaration>}
 ****************************************/
void struct_declaration_list(Type *type)
{
	uint32_t max_align, offset;
	Symbol *s, **ps;
	/*get the "base symbol":sym_struct */
	s = type->ref;

	getToken();

	if (s->relation != STRUCT_NOT_DEFINED) {
		error("struct is defined");
	}

	max_align = 1;
	ps = &s->next;
	offset = 0;

	while (cur_token != tk_END) {
		/* If there is a struct:
		 * struct ex{int a[5][7]; int b,c,d; char e; };
		 * the stack will look like:
		 *     #stack high
		 *         e
		 *         d
		 *         c
		 *         b
		 *         a
		 *         5
		 *         7
		 *     #stack low
		 */
		struct_declaration(&max_align, &offset, &ps);
	}
	skip(tk_END);

	s->relation = calc_align(offset, max_align); /*struct size for the "base symbol"*/
	s->storage_type = max_align;/*struct alignment for the "base symbol"*/
}

/*return size of the type*/
uint32_t type_size(Type *type, uint32_t *align)
{
	Symbol *s;
	uint32_t base_type;

	base_type = (type->data_type & T_BTYPE);
	switch (base_type) {
	case T_STRUCT:
		s = type->ref;
		*align = s->storage_type;
		return s->relation;
	case T_PTR:
		if (type->data_type & T_ARRAY) {
			s = type->ref;
			return (type_size(&s->type, align) * s->relation);
		} else {
			*align = PTR_SIZE;
			return PTR_SIZE;
		}
	case T_INT:
		*align = 4;
		return 4;
	default:/*char,void,function*/
		*align = 1;
		return 1;
	}
}


/****************************************
 * <struct_declaration>::=<type_specifier><struct_declaration_list><tk_SEMICOLON>
 * <struct_declaration_list>::=<declarator>{<tk_COMMA><declarator>}
 *
 * Above are equivalent to:
 *
 * <struct_declaration>::=<type_specifier><declarator>{<tk_COMMA><declarator>}<tk_SEMICOLON>
 ****************************************/
void struct_declaration(uint32_t *max_align, uint32_t *offset, Symbol ***ps)
{
	uint32_t tk, size, align, force_align;
	Symbol *ss;
	Type type_1, base_type;

	type_specifier(&base_type);

	while (1) {
		tk = 0;
		type_1 = base_type;
		declarator(&type_1, &tk, &force_align);
		size = type_size(&type_1, &align);

		if (force_align & ALIGN_SET) {
			align = force_align & ~ALIGN_SET;
		}
		*offset = calc_align(*offset, align);

		if (align > *max_align) {
			*max_align = align;
		}

		ss = sym_push(tk | JC_MEMBER, &type_1, NOT_SPECIFIED, *offset);
		*offset += size;
		**ps = ss;
		/*if same type has several declaration, assign to the "next"
		 *Ex:
		 * struct name{ int a,b,c;};
		 * sym_a->next = sym_b
		 * sym_b->next = sym_c
		 * sym_c->next = NULL
		 */
		*ps = &ss->next;

		if (cur_token == tk_SEMICOLON) {
			break;
		}
		skip(tk_COMMA);
	}
	skip(tk_SEMICOLON);
}

static void struct_member_alignment(uint32_t *force_align)
{
	/*JCC does not support alignment keyword*/
	*force_align = 1;
}



/****************************************
 * <declarator>::={<pointer>}<direct_declarator>
 * <pointer>::=<tk_STAR>
 *
 * Above are equivalent to :
 *
 * <declarator>::={<tk_STAR>}<direct_declarator>
 ****************************************/
void declarator(Type *type, uint32_t *tk, uint32_t *force_align)
{
	while (cur_token == tk_STAR) {
		mk_pointer(type);
		getToken();
	}
	if (force_align) {
		struct_member_alignment(force_align);
	}
	direct_declarator(type, tk);
}


/****************************************
 * <direct_declarator>::=<IDENTIFIER><direct_declarator_postfix>
 ****************************************/
void direct_declarator(Type *type, uint32_t *tk)
{
	if (cur_token >= tk_IDENT) {
		*tk = cur_token;
		getToken();
	} else {
		expect("Identifier!");
	}
	direct_declarator_postfix(type);
}



/****************************************
 * <direct_declarator_postfix>::={<tk_openBR><tk_cINT><tk_closeBR>
 *                                | <tk_openBR><tk_closeBR>
 *                                | <tk_openPA><parameter_type_list><tk_closePA>
 *                                | <tk_openPA><tk_closePA>
 *                               }
 ****************************************/
void direct_declarator_postfix(Type *type)
{
	int relation;
	Symbol *s;

	if (cur_token == tk_openPA) {
		/*function declaration*/
		parameter_type_list(type);
	} else if (cur_token == tk_openBR) {
		/*declarating an array as struct member*/
		getToken();
		relation = NOT_DEFINED;
		if (cur_token == tk_cINT) {
			getToken();
			relation = tkValue;
		}
		skip(tk_closeBR);
		direct_declarator_postfix(type);/*only if multidimentional array will do something*/
		s = sym_push(JC_ANOM, type, NOT_SPECIFIED, relation);
		type->data_type = T_ARRAY | T_PTR;
		type->ref = s;
	}
}


/****************************************
 * <parameter_type_list>::=<parameter_list> | <parameter_list><tk_COMMA><tk_ELLIPSIS>
 * <parameter_list>::<parameter_declaration>{<tk_COMMA><parameter_declaration>}
 * <parameter_declaration>::=<type_specifier>{<declarator>}
 *
 * Above are equivalent to:
 *
 * <parameter_type_list>::=<type_specifier>{<declarator>}{<tk_COMMA><type_specifier>{declarator}}{<tk_COMMA><tk_ELLIPSIS>}
 ****************************************/
void parameter_type_list(Type *type)
{
	uint32_t tk = 0;
	Symbol *s, **pLast, *pFirst;
	Type pt;

	getToken();

	pFirst = NULL;
	pLast = &pFirst;

	while (cur_token != tk_closePA) {
		if (!type_specifier(&pt)) {
			error("Invalid type identifier!");
		}

		declarator(&pt, &tk, NULL);
		s = sym_push(tk | JC_PARAMS, &pt, NOT_SPECIFIED, NOT_SPECIFIED);
		/* If there are several parameters,the "next" pointer points to the next parameter
		 * Ex.
		 * int func(int x,int y,int z){}
		 * sym_x->next = sym_y;
		 * sym_y->next = sym_z;
		 * sym_z->next = NULL;
		 */
		*pLast = s;
		pLast = &s->next;


		if (cur_token == tk_closePA) {
			break;
		}

		skip(tk_COMMA);

		if (cur_token == tk_ELLIPSIS) {
			getToken();
			break;
		}
	}
	skip(tk_closePA);

	s = sym_push(JC_ANOM, type, NOT_SPECIFIED, NOT_SPECIFIED);
	s->next = pFirst;/*Make "anomyous symbol->next" point to the "first parameter symbol" */
	type->data_type = T_FUNC;
	type->ref = s;
}


/****************************************
 * <funcbody>::=<compound_statement>
 ****************************************/
void funcbody(Symbol *sym)
{
	/*put an anonymous symbol in local symbol table*/
	sym_direct_push(&local_sym_stack, JC_ANOM, &int_type, NOT_SPECIFIED);

	/* reset the FP offset variable */
	clearFP_offset();

	/*generating function prolog*/
	genFuncProlog(sym);

	compound_statement(NULL, NULL);

	/*generating function epilog*/
	FreeAllReg();
	genFuncEpilog(sym);

	/*clear local symbol stack*/
	sym_pop(&local_sym_stack, NULL);
}


/****************************************
 * <initializer>::=<assignment_expression>
 ****************************************/
void initializer(Type *type)
{
	if (type->data_type & T_ARRAY) {
		getToken();
	} else {
		assignment_expression();
	}
}


void mk_pointer(Type *type)
{
	Symbol *s;
	s = sym_push(JC_ANOM, type, NOT_SPECIFIED, PTR_NOT_DEFINED);
	type->data_type = T_PTR;
	type->ref = s;
}











