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
#ifndef __genVar_H_
#define __genVar_H_

#include "symbol.h"
#include "global_var.h"


#define asmPrintf(...)  fprintf(output_File,__VA_ARGS__)

#define dSIZE_8bits      "byte"
#define dSIZE_32bits     "long"
#define dSIZE_64bits     "quad"


void genGlobalVar(Symbol *sym);
void clearFP_offset(void);
void updateSYM_FPoffset(Symbol *sym, uint32_t size);
void genLocalVar(Symbol *sym);
void genVar(void);
void genAssign(void);


#endif

