/*
 * =============================================================================
 * 
 *       Filename:  list_head_ex.h
 * 
 *    Description: Write a doubly linked list by using list_head structure 
 * 
 *        Version:  1.0
 *        Created:  Fri Oct 19 14:17:58 GMT 2007 
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  Adrian Huang 
 *       Web Site:  http://adrianhuang.blogspot.com/
 * 
 * =============================================================================
 */

#ifndef  LIST_HEAD_EX_INC
#define  LIST_HEAD_EX_INC

#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/jiffies.h>
#include <linux/list.h>		/* list_head structure	*/

#define	NR_STDS	5	

struct student {
	char				name[16];
	int					id;
	struct list_head	list;
};

typedef enum _Boolean { FALSE = 0, TRUE = 1 } LHBOOL; // list head boolean

#endif   /* ----- #ifndef LIST_HEAD_EX_INC  ----- */

