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

#ifndef __tkWord_Hash_H_
#define __tkWord_Hash_H_
#include <stdint.h>
#include "token.h"
#include "symbol.h"

/*For hash_func*/
#define ROT32(x, y) ((x << y) | (x >> (32 - y)))
#define HASH_SEED 1998
#define MAX_KEY 1024
uint32_t hash_func(const char *key, uint32_t len);

/* single word definition for storage */
typedef struct tkWord {
	uint32_t tkCode;       /*token encoding*/
	struct tkWord *next;   /*pointer to next token words if hash conflict happens*/
	char *str;             /*token string(how to spell)*/
	uint32_t str_len;      /*token string length*/
	Symbol *sym_struct;
	Symbol *sym_identifier;
} tkWord;

tkWord *tkW_direct_insert(tkWord *tkp);
tkWord *tkW_search(char *p, uint32_t key_no);
tkWord *tkW_insert(char *p);

#endif
