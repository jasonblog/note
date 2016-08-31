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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "lex.h"
#include "global_var.h"
#include "token.h"
#include "jcc.h"
#include "declaration.h"
#include "stack.h"
#include "symbol.h"
#include "genFunc.h"
#include "reg.h"
#include "genVar.h"
#include "exception.h"

void close_FILE(void)
{
	char c;

	fclose(output_File);
	fclose(func_File);

	/*open the output file*/
	char output_name[20];
	strcpy(output_name, cur_filename);
	output_name[strlen(cur_filename) - 2] = '\0';
	strcat(output_name, ".s");
	output_File = fopen(output_name, "a+");
	if (!output_File) {
		interERROR("Opening output file error for copy!");
		exit(EXIT_FAILURE);
	}
	/*open the file for functions*/
	func_File = fopen(func_File_name, "r");
	if (!func_File) {
		interERROR("Opening temportary function file error for copy!");
		exit(EXIT_FAILURE);
	}

	while ((c = fgetc(func_File)) != EOF) {
		fputc(c, output_File);
	}
	fclose(cur_File);
	fclose(output_File);
	fclose(func_File);
}

int main(int argc, char **argv)
{
	cur_File = fopen(argv[1], "r");
	if (!cur_File) {
		interERROR("Opening JCC source file failed!\n");
		exit(EXIT_FAILURE);
	}
	cur_filename = argv[1];

	/*open the output file*/
	char output_name[20];
	strcpy(output_name, cur_filename);
	output_name[strlen(cur_filename) - 2] = '\0';
	strcat(output_name, ".s");
	output_File = fopen(output_name, "w");
	if (!output_File) {
		interERROR("Opening output file error!");
		exit(EXIT_FAILURE);
	}
	/*open the file for functions*/
	strcpy(func_File_name, cur_filename);
	func_File_name[strlen(cur_filename) - 2] = '\0';
	strcat(func_File_name, ".f");
	func_File = fopen(func_File_name, "w");
	if (!func_File) {
		interERROR("Opening temportary function file error!");
		exit(EXIT_FAILURE);
	}
	genFileTitle();


	init();

	getCHAR();
	getToken();
	translation_unit();

	cleanup();

	close_FILE();
	printf("Code Generation SUCCESS! File: %s \n\n", argv[1]);
	return 0;
}

void init(void)
{
	cur_line_num = 1;
	init_lex();

	stack_init(&local_sym_stack, 8);
	stack_init(&global_sym_stack, 8);

	int_sym.type.data_type = T_INT;
	int_sym.storage_type = JC_GLOBAL | JC_CONST;
	char_sym.type.data_type = T_CHAR;
	char_sym.storage_type = JC_GLOBAL | JC_CONST;
	ret_sym.type.data_type = T_NOT_TYPE;
	ret_sym.storage_type = JC_RET_REG;


	void_type.data_type = T_VOID;
	int_type.data_type = T_INT;
	char_pointer_type.data_type = T_CHAR;
	mk_pointer(&char_pointer_type);
	default_func_type.data_type = T_FUNC;
	default_func_type.ref = sym_push(JC_ANOM, &void_type, NOT_SPECIFIED, NOT_SPECIFIED);

	RegPoolInit();
}

void cleanup(void)
{
	sym_pop(&global_sym_stack, NULL);
	stack_destroy(&local_sym_stack);
	stack_destroy(&global_sym_stack);

	printf("\ntkTable.count = %d \n", tkTable.count);

	for (uint32_t i = tk_IDENT; i < tkTable.count; i++) {
		free(tkTable.data[i]);
	}
	free(tkTable.data);
}





