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
#include <stdlib.h>
#include "debug.h"
#include "dynSTR.h"

void dynSTR_init(dynSTR *pSTR, uint32_t init_size)
{
	if (pSTR != NULL) {
		pSTR->data = (char*)malloc(sizeof(char) * init_size);
		pSTR->capacity = init_size;
		pSTR->count = 0;
	} else {
		DBG_printf("In dynSTR_init(),pSTR must not be NULL!\n");
	}
}

void dynSTR_free(dynSTR *pSTR)
{
	if (pSTR != NULL) {
		if (pSTR->data) {
			free(pSTR->data);
		}
		pSTR->count = 0;
		pSTR->capacity = 0;
	} else {
		DBG_printf("In dynSTR_free(),pSTR must not be NULL!\n");
	}
}

void dynSTR_reInit(dynSTR *pSTR)
{
	dynSTR_free(pSTR);
	dynSTR_init(pSTR, 8); /*Init size = 8 bytes*/
}

static void dynSTR_realloc(dynSTR *pSTR, uint32_t new_size)
{
	if ((pSTR == NULL) || (pSTR->data == NULL) || (pSTR->capacity == 0)) {
		DBG_printf("In dynSTR_realloc,pSTR must have data or capacity!\n");
	}

	char *data;
	uint32_t capacity = 0;
	capacity = pSTR->capacity;

	while (capacity < new_size) {
		capacity *= 2;
	}

	data = realloc(pSTR->data, capacity);
	if (!data) {
		perror("dynSTR_realloc Failed!");
	}
	pSTR->data = data;
	pSTR->capacity = capacity;
}

void dynSTR_charConcat(dynSTR *pSTR, int ch)
{
	uint32_t count = 0;
	count = pSTR->count + 1;

	if (count > pSTR->capacity) {
		dynSTR_realloc(pSTR, count);
	}

	((char *)pSTR->data)[count - 1] = ch;
	pSTR->count = count;
}





