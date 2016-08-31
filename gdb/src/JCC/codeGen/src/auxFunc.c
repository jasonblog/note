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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "auxFunc.h"
#include "global_var.h"
#include "exception.h"

/*for if statement*/
void replace_output(char *target_original, char *target_new)
{
	char read_buffer[1024];
	char write_buffer[1024];
	char *str_loc = NULL;
	uint32_t str_index;
	char c;
	FILE *output_temp_file;
	output_temp_file = fopen("temp.s", "w+");
	if (!output_temp_file) {
		interERROR("Opening temportary file error!");
		exit(EXIT_FAILURE);
	}

	/*put the results in output_temp_file*/
	fclose(func_File);
	func_File = fopen(func_File_name, "r");
	if (!func_File) {
		interERROR("Opening %s error in replace_output!", func_File_name);
		exit(EXIT_FAILURE);
	}

	while (fgets(read_buffer, sizeof(read_buffer), func_File) != NULL) {
		str_loc = strstr(read_buffer, target_original);
		if (str_loc != NULL) {
			str_index = str_loc - read_buffer;
			strncpy(write_buffer, read_buffer, str_index);
			write_buffer[str_index] = '\0';
			strcat(write_buffer, target_new);
		} else {
			strcpy(write_buffer, read_buffer);
		}
		fputs(write_buffer, output_temp_file);
	}
	fflush(output_temp_file);
	fclose(func_File);
	func_File = fopen(func_File_name, "w");

	if (!func_File) {
		interERROR("Opening %s error in replace_output!", func_File_name);
		exit(EXIT_FAILURE);
	}

	/*copy the output_temp_file back to target_file*/
	rewind(output_temp_file);
	while ((c = fgetc(output_temp_file)) != EOF) {
		fputc(c, func_File);
	}

	fflush(func_File);
	fclose(output_temp_file);
}



