/*
boot.c:
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

Tzu Hang<karose.tzu@gmail.com> modifed to miniOS 
*/

#include "process.h"
#include "scheduler.h"



typedef void (*init_func)(void);

#define UFCON0	((volatile unsigned int *)(0x20201000))

void helloworld(void){
	const char *p="HelloWorld\n";
	while(*p){
		*UFCON0=*p++;
	};
}

void test_mmu(void){
	const char *p="test_mmu\n";
	while(*p){
		*(volatile unsigned int *)0xa0201000=*p++;
	};
}


static init_func init[]={
	helloworld,
	0,
};

void main_endloop(void)
{
	printk("\nPrefetch abort done");

	/* Repeatedly halt the CPU and wait for interrupt */
	while(1)
		asm volatile("mcr p15,0,r0,c7,c0,4" : : : "r0");

}



void plat_boot(void){
	int i;
	for(i=0;init[i];i++){
		init[i]();
	}
	   
        printk("PRINTK TEST\n");

        enable_mmu2();
        test_mmu();
        
        //Timer interrupt init.
        //IRQ_init();

        //memory management Buddy system
        init_page_map();
        char *p1,*p2,*p3,*p4;
	p1=(char *)get_free_pages(0,6);
	printk("the return address of get_free_pages %x\n",p1);
	p2=(char *)get_free_pages(0,6);
	printk("the return address of get_free_pages %x\n",p2);
	put_free_pages(p2,6);
	put_free_pages(p1,6);
	p3=(char *)get_free_pages(0,7);
	printk("the return address of get_free_pages %x\n",p3);
	p4=(char *)get_free_pages(0,7);
	printk("the return address of get_free_pages %x\n",p4);

	
	create_main_process();

	// Create three sample processes with name and address of function
	fork("Sample process 1", &sample_process_1);
	fork("Sample process 2", &sample_process_2);
        fork("Sample process 3", &sample_process_2);

        IRQ_init();
        while(1);
}
