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

#include "global_var.h"
#include "token.h"

/*Lexical analysis*/
tkWord *tk_hashTable[MAX_KEY];
dynARR tkTable;
dynSTR sourceSTR;
dynSTR cur_tkSTR;
int tkValue;
uint32_t cur_token;
uint32_t compiler_stage;
uint32_t cur_line_num;
char *cur_filename;
char cur_CHAR;
FILE *cur_File;

/*Semantic analysis*/
Stack global_sym_stack;
Stack local_sym_stack;

Type char_pointer_type;
Type void_type;
Type int_type;
Type default_func_type;

Symbol char_sym;
Symbol int_sym;
Symbol ret_sym;

/*Code generation*/
FILE *output_File;
FILE *func_File;
char func_File_name[30];

uint32_t FP_offset = 0;
uint32_t CodeGenStatus;
Operand opStack[opStack_Size];
Operand *opTop = opStack;
uint32_t const_STR_index = 0;
REG reg_pool[REG_MAX - REG_BASE];
uint32_t condtion_label_count = 0;
uint32_t for_label_count = 0;

