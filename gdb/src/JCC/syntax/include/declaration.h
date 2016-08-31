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

#ifndef __DECLARATION_H_
#define __DECLARATION_H_
#include "symbol.h"

#define ALIGN_SET 0x100
#define PTR_SIZE  4

void translation_unit(void);
void external_declaration(uint32_t storage_type);
int type_specifier(Type *type);
void struct_specifier(Type *type);
void struct_declaration_list(Type *type);
void struct_declaration(uint32_t *max_align, uint32_t *offset, Symbol ***ps);
void declarator(Type *type, uint32_t *tk, uint32_t *force_align);
void direct_declarator(Type *type, uint32_t *tk);
void direct_declarator_postfix(Type *type);
void parameter_type_list(Type *type);
void funcbody(Symbol *sym);
void initializer(Type *type);
void mk_pointer(Type *t);
uint32_t type_size(Type *type, uint32_t *align);

#endif
