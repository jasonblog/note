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

Hzu Hang modifed.
*/




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



void plat_boot(void){
	int i;
	for(i=0;init[i];i++){
		init[i]();
	}
	   
             printk("PRINTK TEST\n");
             enable_mmu2();
             test_mmu();

	      printk("Finish TEST\n");	
	while(1);
}
