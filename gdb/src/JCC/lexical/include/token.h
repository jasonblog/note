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

#ifndef __TOKEN_H_
#define __TOKEN_H_
#include <stdint.h>

typedef enum {
	/*OPERAND and SEPARATION SYMBOL*/
	tk_PLUS,     // +
	tk_MINUS,    // -
	tk_STAR,     // *
	tk_DIVIDE,   // /
	tk_MOD,      // %
	tk_EQ,       // ==
	tk_NEQ,      // !=
	tk_LT,       // <
	tk_LEQ,      // <=
	tk_GT,       // >
	tk_GEQ,      // >=
	tk_ASSIGN,   // =
	tk_POINTTO,  // ->
	tk_DOT,      // .
	tk_AND,      // &
	tk_openPA,   // (
	tk_closePA,  // )
	tk_openBR,   // [
	tk_closeBR,  // ]
	tk_BEGIN,    // {
	tk_END,      // }
	tk_SEMICOLON,// ;
	tk_COMMA,    // ,
	tk_ELLIPSIS, // ...
	tk_EOF,      // END OF FILE

	/*CONSTANTS*/
	tk_cINT,     // const. integer
	tk_cCHAR,    // const. char
	tk_cSTR,     // const. STRING

	/*KEY WORDS*/
	kw_CHAR,     // char
	kw_INT,      // int
	kw_VOID,     // void
	kw_STRUCT,   // struct

	kw_IF,       // if
	kw_ELSE,     // else
	kw_FOR,      // for
	kw_CONTINUE, // continue
	kw_BREAK,    // break
	kw_RETURN,   // return
	kw_SIZEOF,   // sizeof

	/*SPECIAL KEYWORDS*/
	kw_PRINTF,   //printf
	kw_SCANF,    //scanf

	/*IDENTIFIERS*/
	tk_IDENT
} TOKEN;

void getCHAR(void);
void getToken(void);
char *get_tkSTR(uint32_t tk_code);

#endif
