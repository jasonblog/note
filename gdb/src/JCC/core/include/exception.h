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

#ifndef __EXCEPTION_H
#define __EXCEPTION_H
#include <stdint.h>
#include <stdarg.h>
#include "dynARR.h"
#include "token.h"

typedef enum {
	LEVEL_WARNING,
	LEVEL_ERROR,
	INTERNAL_ERROR,
} ErrorLevel;

void warning(char *fmt, ...);
void error(char *fmt, ...);
void expect(char *msg);
void skip(uint32_t tk);
void interERROR(char *fmt, ...);

#endif
