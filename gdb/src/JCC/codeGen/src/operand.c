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
#include "operand.h"
#include "exception.h"
#include "global_var.h"

void operand_push(Symbol *sym, int value)
{
	if (opTop >= opStack + (opStack_Size - 1)) {
		error("opStack memory allocation failed");
	}

	opTop++;
	opTop->value = value;
	opTop->sym = sym;/*for const int/char, this will be NULL*/
	opTop->tk_code = NOT_SPECIFIED;
}


void operand_pop(void)
{
	opTop--;
}


void operand_TopSwap(void)
{
	Operand temp;
	temp = opTop[0];
	opTop[0] = opTop[-1];
	opTop[-1] = temp;
}



