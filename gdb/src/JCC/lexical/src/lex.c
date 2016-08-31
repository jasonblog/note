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
#include "lex.h"
#include "global_var.h"
#include "token.h"
#include "tkWord_Hash.h"
#include "exception.h"
#include "dynARR.h"

void init_lex(void)
{
	static tkWord keywords[] = {
		{tk_PLUS,      NULL,   "+",                strlen("+") + 1},
		{tk_MINUS,     NULL,   "-",                strlen("-") + 1},
		{tk_STAR,      NULL,   "*",                strlen("*") + 1},
		{tk_DIVIDE,    NULL,   "/",                strlen("/") + 1},
		{tk_MOD,       NULL,   "%",                strlen("%") + 1},
		{tk_EQ,        NULL,   "==",               strlen("==") + 1},
		{tk_NEQ,       NULL,   "!=",               strlen("!=") + 1},
		{tk_LT,        NULL,   "<",                strlen("<") + 1},
		{tk_LEQ,       NULL,   "<=",               strlen("<=") + 1},
		{tk_GT,        NULL,   ">",                strlen(">") + 1},
		{tk_GEQ,       NULL,   ">=",               strlen(">=") + 1},
		{tk_ASSIGN,    NULL,   "=",                strlen("=") + 1},
		{tk_POINTTO,   NULL,   "->",               strlen("->") + 1},
		{tk_DOT,       NULL,   ".",                strlen(".") + 1},
		{tk_AND,       NULL,   "&",                strlen("&") + 1},
		{tk_openPA,    NULL,   "(",                strlen("(") + 1},
		{tk_closePA,   NULL,   ")",                strlen(")") + 1},
		{tk_openBR,    NULL,   "[",                strlen("[") + 1},
		{tk_closeBR,   NULL,   "]",                strlen("]") + 1},
		{tk_BEGIN,     NULL,   "{",                strlen("{") + 1},
		{tk_END,       NULL,   "}",                strlen("}") + 1},
		{tk_SEMICOLON, NULL,   ";",                strlen(";") + 1},
		{tk_COMMA,     NULL,   ",",                strlen(",") + 1},
		{tk_ELLIPSIS,  NULL,   "...",              strlen("...") + 1},
		{tk_EOF,       NULL,   "End_Of_File",      strlen("End_Of_File") + 1},

		{tk_cINT,      NULL,   "Constant Integer", strlen("Constant Integer") + 1},
		{tk_cCHAR,     NULL,   "Constant CHAR",    strlen("Constant CHAR") + 1},
		{tk_cSTR,      NULL,   "Constant String",  strlen("Constant String") + 1},

		{kw_CHAR,      NULL,   "char",             strlen("char") + 1},
		{kw_INT,       NULL,   "int",              strlen("int") + 1},
		{kw_VOID,      NULL,   "void",             strlen("void") + 1},
		{kw_STRUCT,    NULL,   "struct",           strlen("struct") + 1},

		{kw_IF,        NULL,   "if",               strlen("if") + 1},
		{kw_ELSE,      NULL,   "else",             strlen("else") + 1},
		{kw_FOR,       NULL,   "for",              strlen("for") + 1},
		{kw_CONTINUE,  NULL,   "continue",         strlen("continue") + 1},
		{kw_BREAK,     NULL,   "break",            strlen("break") + 1},
		{kw_RETURN,    NULL,   "return",           strlen("return") + 1},
		{kw_SIZEOF,    NULL,   "sizeof",           strlen("sizeof") + 1},

		{kw_PRINTF,    NULL,   "printf",           strlen("printf") + 1},
		{kw_SCANF,     NULL,   "scanf",            strlen("scanf") + 1},

		/*For the foe loop below*/
		{0,           NULL,   NULL,          0}
	};

	dynARR_init(&tkTable, 8);
	for (tkWord *tkp = &keywords[0]; tkp->str != NULL; tkp++) {
		tkW_direct_insert(tkp);
	}
}



