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

#ifndef __GLOBAL_VAR_H
#define __GLOBAL_VAR_H

#include <stdint.h>
#include <stdio.h>
#include "token.h"
#include "tkWord_Hash.h"
#include "dynARR.h"
#include "dynSTR.h"
#include "stack.h"
#include "symbol.h"
#include "operand.h"
#include "reg.h"

/*Lexcial analysis*/
extern tkWord *tk_hashTable[MAX_KEY];
extern dynARR tkTable;
/*In get_tkSTR(),if the target token is constant string/character/number, we get string from dynSTR sourceSTR */
extern dynSTR sourceSTR;
/* For identifying (new) token purpose. */
extern dynSTR cur_tkSTR;
extern int tkValue;
extern uint32_t cur_token;
extern uint32_t compiler_stage;
extern uint32_t cur_line_num;
/*Indicating where are we in the JCC source file.*/
extern char cur_CHAR;
/*The target JCC source file*/
extern FILE *cur_File;
extern char *cur_filename;
/**************************************************************/

/*Semantic analysis*/
extern Stack global_sym_stack;
extern Stack local_sym_stack;

extern Type char_pointer_type;
extern Type void_type;
extern Type int_type;
extern Type default_func_type;

extern Symbol char_sym;
extern Symbol int_sym;
extern Symbol ret_sym;/*special symbol used for record register info*/

/*Code generation*/
extern FILE *output_File;
extern FILE *func_File;
extern char func_File_name[30];
extern uint32_t FP_offset;/*local variable frame pointer offset*/

#define opStack_Size 512
extern Operand opStack[opStack_Size];
extern Operand *opTop;

extern uint32_t const_STR_index;

extern REG reg_pool[REG_MAX - REG_BASE]; /*reg_pool[0] will not be used*/

/*if label count,ex:  .L2 */
extern uint32_t condtion_label_count;/*for "if"*/
extern uint32_t for_label_count;/*for "for"*/

/**************************************************************/
#endif
