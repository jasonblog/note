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

#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "exception.h"

void stack_init(Stack *stack, uint32_t init_size)
{
	stack->base = (void**)malloc(sizeof(void**)*init_size);
	if (!stack->base) {
		error("Stack memory allocation failed!");
	} else {
		stack->top = stack->base;
		stack->stack_size = init_size;
	}
}


void *stack_push(Stack *stack, void *element, uint32_t element_size)
{
	uint32_t new_size;
	if ((stack->top) >= (stack->base + stack->stack_size)) {
		new_size = stack->stack_size * 2;
		stack->base = (void**)realloc(stack->base, sizeof(void**)*new_size);

		if (!stack->base) {
			return NULL;
		}
		stack->top = stack->base + stack->stack_size;
		stack->stack_size = new_size;
	}
	*stack->top = (void*)malloc(element_size);
	memcpy(*stack->top, element, element_size);
	stack->top++;
	return *(stack->top - 1);
}


void stack_pop(Stack *stack)
{
	if (stack->top > stack->base) {
		stack->top = stack->top - 1;
		free(*(stack->top));
	}
}


void *stack_get_top(Stack *stack)
{
	void **element;
	if (stack->top > stack->base) {
		element = stack->top - 1;
		return *element;
	} else {
		return NULL;
	}
}


uint32_t stack_is_empty(Stack *stack)
{
	if (stack->top == stack->base) {
		return STACK_IS_EMPTY;
	} else {
		return STACK_IS_NOT_EMPTY;
	}
}


void stack_destroy(Stack *stack)
{
	for (void **element = stack->base; element < stack->top; element++) {
		free(*element);
	}

	if (stack->base) {
		free(stack->base);
	}
	stack->base = NULL;
	stack->top = NULL;
	stack->stack_size = 0;
}



























