/*
 * =============================================================================
 *
 *       Filename:  list_head_ex.c
 *
 *    Description:  Write a doubly linked list by using list_head structure
 *
 *        Version:  1.0
 *        Created:  Fri Oct 19 14:14:57 GMT 2007
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Adrian Huang
 *        Web Site:  http://adrianhuang.blogspot.com/
 *
 * ============================================================================
 */

#include "list_head_ex.h"

int __init list_head_init(void)
{
	struct list_head	std_head, *next;
	struct student		sl[NR_STDS]; 		// student list
	struct student		*studentP; 	// student pointer
	int					idx;

	/* init std_head */
	INIT_LIST_HEAD(&std_head);

	/* Check whether the std_head is empty */
	if(list_empty(&std_head) == TRUE) {
		printk(KERN_INFO "[Adrian] std_head is NULL\n");
	}

	/* Add each element to student head (std_head) */
	for(idx=0;idx<sizeof(sl)/sizeof(struct student);idx++) {
		sprintf(sl[idx].name, "Adrian Huang");
		sl[idx].id = idx + 1;
		list_add(&sl[idx].list, &std_head);
	}	
	
	printk(KERN_INFO "======dump all elements of the list by invoking "
					"'list_for_each'=========\n");
	/* Traverse all elements of the list */
	list_for_each(next, &std_head) {
		studentP = (struct student *) list_entry(next, struct student, list);
		printk(KERN_INFO "[Adrian] name: %s, id: %d\n",
							 studentP->name, studentP->id);
	}


	printk(KERN_INFO "\n======dump all elements of the list by invoking "
					"'list_entry'=========\n");
	/* Traverse all elements of the list and delete each of it */	
	for(idx=0;idx<sizeof(sl)/sizeof(struct student);idx++) {
		studentP = (struct student *) list_entry(std_head.next,
									 struct student, list);

		printk(KERN_INFO "[Adrian] name: %s, id: %d\n",
							 studentP->name, studentP->id);
	
		list_del(std_head.next);
	}
			
	return 0;
}

void __exit list_head_exit(void)
{
	;
}

module_init(list_head_init);
module_exit(list_head_exit);

