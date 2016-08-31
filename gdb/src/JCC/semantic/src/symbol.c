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
#include <stddef.h>
#include "symbol.h"
#include "token.h"
#include "stack.h"
#include "tkWord_Hash.h"
#include "global_var.h"
#include "exception.h"

Symbol *sym_direct_push(Stack *ss, uint32_t modified_tk, Type *type, int relation)
{
	Symbol s, *p;
	s.tk_code = modified_tk;
	s.type.data_type = type->data_type;
	s.type.ref = type->ref;
	s.relation = relation;
	s.next = NULL;
	p = (Symbol*)stack_push(ss, &s, sizeof(Symbol));
	return p;
}

/*the passed modified_tk does not mean TOKEN,it may be encode with Symbol type*/
Symbol *sym_push(uint32_t modified_tk, Type *type, uint32_t storage_type, int relation)
{
	Symbol *ps, **pps;
	tkWord *ts;
	Stack *ss;

	if (stack_is_empty(&local_sym_stack) == STACK_IS_NOT_EMPTY) {
		ss = &local_sym_stack;
	} else {
		ss = &global_sym_stack;
	}

	ps = sym_direct_push(ss, modified_tk, type, relation);
	ps->storage_type = storage_type;

	/*Do NOT record struct member and anonymous symbol*/
	if ((modified_tk & JC_STRUCT) || (modified_tk < JC_ANOM)) {
		ts = (tkWord*)tkTable.data[(modified_tk & ~JC_STRUCT)];
		if (modified_tk & JC_STRUCT) {
			pps = &ts->sym_struct;
		} else {
			pps = &ts->sym_identifier;
		}
		/*Make prev point to itself,and return*/
		ps->prev = *pps;
		*pps = ps;
	}
	return ps;
}


Symbol *func_sym_push(uint32_t tk, Type *type)
{
	Symbol *s, **ps;
	s = sym_direct_push(&global_sym_stack, tk, type, NOT_SPECIFIED);

	ps = &((tkWord*)tkTable.data[tk])->sym_identifier;

	/*If symbol shares the same name,put function symbol at last place*/
	while (*ps != NULL) {
		ps = &(*ps)->prev;
	}
	s->prev = NULL;
	*ps = s;
	return s;
}


Symbol *var_sym_put(Type *type, uint32_t storage_type, uint32_t tk, int value)
{
	Symbol *sym = NULL;
	/*local variables*/
	if ((storage_type & JC_ValMASK) == JC_LOCAL) {
		sym = sym_push(tk, type, storage_type, value);
		/*global varable*/
	} else if (tk && ((storage_type & JC_ValMASK) == JC_GLOBAL)) {
		sym = sym_search(tk);
		if (sym) {
			error("%s redefinition!\n", ((tkWord*)tkTable.data[tk])->str);
		} else {
			sym = sym_push(tk, type, storage_type | JC_SYM, value);
		}
	}/*else:const string symbol*/
	else {
		sym = sym_push(tk, type, storage_type, value);
	}
	return sym;
}



void sym_pop(Stack *stack, Symbol *sym)
{
	Symbol *s, **ps;
	tkWord *ts;
	uint32_t modified_tk;

	s = (Symbol*)stack_get_top(stack);
	while (s != sym) {
		modified_tk = s->tk_code;

		/*update sym_struct and sym_identifier in the tkTable*/
		if ((modified_tk & JC_STRUCT) || (modified_tk < JC_ANOM)) {
			ts = (tkWord*)tkTable.data[(modified_tk & ~JC_STRUCT)];
			if (modified_tk & JC_STRUCT) {
				ps = &ts->sym_struct;
			} else {
				ps = &ts->sym_identifier;
			}
			*ps = s->prev;
		}
		stack_pop(stack);
		s = (Symbol*)stack_get_top(stack);
	}
}


Symbol *struct_search(uint32_t tk)
{
	if (tk >= tkTable.count) {
		return NULL;
	} else {
		return ((tkWord*)tkTable.data[tk])->sym_struct;
	}
}


Symbol *sym_search(uint32_t tk)
{
	if (tk >= tkTable.count) {
		return NULL;
	} else {
		return ((tkWord*)tkTable.data[tk])->sym_identifier;
	}
}

























