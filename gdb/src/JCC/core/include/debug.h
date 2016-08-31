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

#ifndef __DEBUG_H_
#define __DEBUG_H_
#include <stdio.h>


#if defined(JCC_DEBUG)
#define DBG_printf(...)  printf(__VA_ARGS__)
#else
#define DBG_printf(...)
/*
 * #warning "If you would like to open DEBUG function in JCC,please check the Makefile!"
 */
#endif



#endif
