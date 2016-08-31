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
#include "dynARR.h"

void dynARR_init(dynARR *pARR, uint32_t init_size)
{
	if (pARR != NULL) {
		pARR->data = (void**)malloc(sizeof(char) * init_size);
		pARR->capacity = init_size;
		pARR->count = 0;
	} else {
		DBG_printf("In dynARR_init(),pARR must not be NULL!\n");
	}
}

void dynARR_free(dynARR *pARR)
{
	for (void **p = pARR->data ; pARR->count > 0; p++) {
		if (*p) {
			free(*p);
			pARR->count -= 1;
		}
	}
	pARR->data = NULL;
}

int dynARR_search(dynARR *pARR, int key)
{
	int **p;
	p = (int **)pARR->data;
	for (int i = 0; i < pARR->count ; p++) {
		if (key == **p) {
			return i;
		}
	}
	return -1;
}

static void dynARR_realloc(dynARR *pARR, uint32_t new_size)
{
	if ((pARR == NULL) || (pARR->data == NULL) || (pARR->capacity == 0)) {
		DBG_printf("In dynARR_realloc,pSTR must have data or capacity!\n");
	}

	void *data;
	uint32_t capacity = 0;
	capacity = pARR->capacity;

	while (capacity < new_size) {
		capacity *= 2;
	}

	data = realloc(pARR->data, capacity);
	if (!data) {
		perror("dynARR_realloc Failed!");
	}
	pARR->data = data;
	pARR->capacity = capacity;
}

void dynARR_add(dynARR *pARR, void *data)
{
	uint32_t count = 0;
	count = pARR->count + 1;

	if (count * sizeof(void*) > pARR->capacity) {
		dynARR_realloc(pARR, count * sizeof(void*));
	}

	pARR->data[count - 1] = data;
	pARR->count = count;
}





