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

#ifndef __dynSTR_H_
#define __dynSTR_H_
#include <stdint.h>

/*dynamic string definition*/
typedef struct {
	uint32_t count;
	char *data;
	uint32_t capacity;
} dynSTR;

void dynSTR_init(dynSTR *pSTR, uint32_t init_size);
void dynSTR_free(dynSTR *pSTR);
void dynSTR_reInit(dynSTR *pSTR);
void dynSTR_charConcat(dynSTR *pSTR, int ch);

#endif
