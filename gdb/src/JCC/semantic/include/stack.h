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

#ifndef __STACK_H_
#define __STACK_H_
#include <stdint.h>

#define STACK_IS_EMPTY 1
#define STACK_IS_NOT_EMPTY 0

typedef struct {
	void **base;
	void **top;
	uint32_t stack_size;
} Stack;


void stack_init(Stack *stack, uint32_t init_size);
void *stack_push(Stack *stack, void *element, uint32_t size);
void stack_pop(Stack *stack);
void *stack_get_top(Stack *stack);
uint32_t stack_is_empty(Stack *stack);
void stack_destroy(Stack *stack);


#endif
