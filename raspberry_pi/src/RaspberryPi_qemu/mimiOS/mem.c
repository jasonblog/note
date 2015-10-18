/*
mem.c:
Copyright (C) 2009  david leels <davidontech@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/.
*/

#define	NULL	((void *)0)

struct list_head {
	struct list_head *next, *prev;
};


static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(struct list_head *new_lst,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new_lst;
	new_lst->next = next;
	new_lst->prev = prev;
	prev->next = new_lst;
}

static inline void list_add(struct list_head *new_lst, struct list_head *head)
{
	__list_add(new_lst, head, head->next);
}

static inline void list_add_tail(struct list_head *new_lst, struct list_head *head)
{
	__list_add(new_lst, head->prev, head);
}

static inline void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(struct list_head * entry)
{
	__list_del(entry->prev,entry->next);
}


static inline void list_remove_chain(struct list_head *ch,struct list_head *ct){
	ch->prev->next=ct->next;
	ct->next->prev=ch->prev;
}

static inline void list_add_chain(struct list_head *ch,struct list_head *ct,struct list_head *head){
		ch->prev=head;
		ct->next=head->next;
		head->next->prev=ct;
		head->next=ch;
}

static inline void list_add_chain_tail(struct list_head *ch,struct list_head *ct,struct list_head *head){
		ch->prev=head->prev;
		head->prev->next=ch;
		head->prev=ct;
		ct->next=head;
}


static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}



#define offsetof(TYPE, MEMBER) ((unsigned int) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
		(type *)( (char *)__mptr - offsetof(type,member) );})


#define list_entry(ptr,type,member)	\
    container_of(ptr, type, member)



#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)



/*memory for dynamic allocation 8MB from 0x00010000 ~ 0x00810000*/
#define _MEM_END	0x00810000
#define _MEM_START	0x00010000

/*left-shift 1 12bits is 0x10000=4096bytes=4KB*/
#define PAGE_SHIFT	(12)
#define PAGE_SIZE	(1<<PAGE_SHIFT)
/*MASK is 0xfffff000*/
#define PAGE_MASK	(~(PAGE_SIZE-1))

#define KERNEL_MEM_END	(_MEM_END)
/*the begin and end of the kernel mem which is needed to be paged. 4K alignment*/
#define KERNEL_PAGING_START	((_MEM_START+(~PAGE_MASK))&((PAGE_MASK)))
/* The number of dynamic memory space divided struct page needed*/
#define	KERNEL_PAGING_END	(((KERNEL_MEM_END-KERNEL_PAGING_START)/(PAGE_SIZE+sizeof(struct page)))*(PAGE_SIZE)+KERNEL_PAGING_START)

/*the number of pages*/
#define KERNEL_PAGE_NUM	((KERNEL_PAGING_END-KERNEL_PAGING_START)/PAGE_SIZE)

/*the memory space uses for store struct page from KERNEL_PAGE_START to _MEM_END*/
#define KERNEL_PAGE_END	_MEM_END
#define KERNEL_PAGE_START	(KERNEL_PAGE_END-KERNEL_PAGE_NUM*sizeof(struct page))


/*page flags*/
#define PAGE_AVAILABLE		0x00
#define PAGE_DIRTY			0x01
#define PAGE_PROTECT		0x02
#define PAGE_BUDDY_BUSY		0x04
#define PAGE_IN_CACHE		0x08


struct page {
	unsigned int vaddr;
	unsigned int flags;
	int order;
	unsigned int counter;
	struct kmem_cache *cachep;
	struct list_head list;//to string the buddy member
};



/*
finally I desided to choose a fixed value,which means users could alloc 1M space at most,
those who need more than 1M should change the macro to reserve enough space for private use.
available from 2^0~2^8 => 1~256 * 4k => from 4k to 1MB
*/
#define MAX_BUDDY_PAGE_NUM	(9)	

#define AVERAGE_PAGE_NUM_PER_BUDDY	(KERNEL_PAGE_NUM/MAX_BUDDY_PAGE_NUM)
#define PAGE_NUM_FOR_MAX_BUDDY	((1<<MAX_BUDDY_PAGE_NUM)-1)

/*init list heads*/
struct list_head page_buddy[MAX_BUDDY_PAGE_NUM];


struct page *virt_to_page(unsigned int addr){
	unsigned int i;
	i=((addr)-KERNEL_PAGING_START)>>PAGE_SHIFT;
	if(i>KERNEL_PAGE_NUM)
		return NULL;
	return (struct page *)KERNEL_PAGE_START+i;
}

void init_page_buddy(void){
	int i;
        printk("init_page_buddy\n");
	for(i=0;i<MAX_BUDDY_PAGE_NUM;i++){
		INIT_LIST_HEAD(&page_buddy[i]);
	}
}


void init_page_map(void){
	int i;
	struct page *pg=(struct page *)KERNEL_PAGE_START;
        printk("init_page_map \n");
        printk("KERNE_FROM:%x to %x \n",KERNEL_PAGING_START,KERNEL_PAGING_END);
        printk("KERNEL_PAGE_STRUCT start:%x \n",KERNEL_PAGE_START);
	init_page_buddy();
	for(i=0;i<(KERNEL_PAGE_NUM);pg++,i++){
/*fill struct page first*/
		pg->vaddr=KERNEL_PAGING_START+i*PAGE_SIZE;	
		pg->flags=PAGE_AVAILABLE;
		pg->counter=0;
		INIT_LIST_HEAD(&(pg->list));


/*make the memory max buddy as possible*/
		if(i<(KERNEL_PAGE_NUM&(~PAGE_NUM_FOR_MAX_BUDDY))){	
/*the following code should be dealt carefully,we would change the order field of a head struct page to the corresponding order,and change others to -1*/
			if((i&PAGE_NUM_FOR_MAX_BUDDY)==0){
				pg->order=MAX_BUDDY_PAGE_NUM-1;
			}else{
				pg->order=-1;
			}
			list_add_tail(&(pg->list),&page_buddy[MAX_BUDDY_PAGE_NUM-1]);
/*the remainder not enough to merge into a max buddy is done as min buddy*/
		}else{
			pg->order=0;
			list_add_tail(&(pg->list),&page_buddy[0]);
		}

	}

}


/*we can do these all because the page structure that represents one page aera is continuous*/
#define BUDDY_END(x,order)	((x)+(1<<(order))-1)
#define NEXT_BUDDY_START(x,order)	((x)+(1<<(order)))
#define PREV_BUDDY_START(x,order)	((x)-(1<<(order)))

/*the logic of this function seems good,no bug reported yet*/
struct page *get_pages_from_list(int order){
	unsigned int vaddr;
	int neworder=order;
	struct page *pg,*ret;
	struct list_head *tlst,*tlst1;
	for(;neworder<MAX_BUDDY_PAGE_NUM;neworder++){
		if(list_empty(&page_buddy[neworder])){
			continue;
		}else{
			pg=list_entry(page_buddy[neworder].next,struct page,list);
			tlst=&(BUDDY_END(pg,neworder)->list);
			tlst->next->prev=&page_buddy[neworder];
			page_buddy[neworder].next=tlst->next;
			goto OUT_OK;
		}
	}
	return NULL;
	
OUT_OK:
	for(neworder--;neworder>=order;neworder--){
		
		tlst1=&(BUDDY_END(pg,neworder)->list);
		tlst=&(pg->list);

		pg=NEXT_BUDDY_START(pg,neworder);
		list_entry(tlst,struct page,list)->order=neworder;

		list_add_chain_tail(tlst,tlst1,&page_buddy[neworder]);
	}
	pg->flags|=PAGE_BUDDY_BUSY;
	pg->order=order;
	return pg;
}



void put_pages_to_list(struct page *pg,int order){
	struct page *tprev,*tnext;
	if(!(pg->flags&PAGE_BUDDY_BUSY)){
		printk("something must be wrong when you see this message,that probably means you are forcing to release a page that was not alloc at all\n");
		return;
	}
	pg->flags&=~(PAGE_BUDDY_BUSY);

	for(;order<MAX_BUDDY_PAGE_NUM;order++){
		tnext=NEXT_BUDDY_START(pg,order);
		tprev=PREV_BUDDY_START(pg,order);
		if((!(tnext->flags&PAGE_BUDDY_BUSY))&&(tnext->order==order)){
			pg->order++;
			tnext->order=-1;
			list_remove_chain(&(tnext->list),&(BUDDY_END(tnext,order)->list));
			BUDDY_END(pg,order)->list.next=&(tnext->list);
			tnext->list.prev=&(BUDDY_END(pg,order)->list);
			continue;
		}else if((!(tprev->flags&PAGE_BUDDY_BUSY))&&(tprev->order==order)){
			pg->order=-1;
			
			list_remove_chain(&(pg->list),&(BUDDY_END(pg,order)->list));
			BUDDY_END(tprev,order)->list.next=&(pg->list);
			pg->list.prev=&(BUDDY_END(tprev,order)->list);
			
			pg=tprev;
			pg->order++;
			continue;
		}else{
			break;
		}
	}
	
	list_add_chain(&(pg->list),&((tnext-1)->list),&page_buddy[order]);
}



void *page_address(struct page *pg){
	return (void *)(pg->vaddr);
}

struct page *alloc_pages(unsigned int flag,int order){
	struct page *pg;
	int i;
	pg=get_pages_from_list(order);
	if(pg==NULL)
		return NULL;
	for(i=0;i<(1<<order);i++){
		(pg+i)->flags|=PAGE_DIRTY;
	}
	return pg;
}

void free_pages(struct page *pg,int order){
	int i;
	for(i=0;i<(1<<order);i++){
		(pg+i)->flags&=~PAGE_DIRTY;
	}
	put_pages_to_list(pg,order);
}

void *get_free_pages(unsigned int flag,int order){
	struct page * page;
	page = alloc_pages(flag, order);
	if (!page)
		return NULL;
	return	page_address(page);
}

void put_free_pages(void *addr,int order){
	free_pages(virt_to_page((unsigned int)addr),order);
}

