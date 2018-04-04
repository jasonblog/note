/*
start.s:
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

Modified by Carlos Hsu <karose.tzu@gmail.com>
*/



.global _start
.global __vector_reset
.global	__vector_undefined
.global	__vector_swi
.global	__vector_prefetch_abort
.global	__vector_data_abort
.global	__vector_reserved
.global	__vector_irq
.global	__vector_fiq

.equ DISABLE_IRQ,		0x80  /* when I bit is set, IRQ is disabled */
.equ DISABLE_FIQ,		0x40  /* when F bit is set, FIQ is disabled */
.equ SYS_MOD,			0x1f  /* System Mode */
.equ IRQ_MOD,			0x12  /* IRQ Mode */
.equ FIQ_MOD,			0x11  /* FIQ Mode */
.equ SVC_MOD,			0x13  /* Supervisor Mode */
.equ ABT_MOD,			0x17  /* Abort Mode */
.equ UND_MOD,			0x1b  /* Undefined Mode */

.equ MEM_SIZE,			0x7A00000
.equ TEXT_BASE,			0x0008000

.equ _SVC_STACK,		(TEXT_BASE+MEM_SIZE-4)
.equ _IRQ_STACK,		(_SVC_STACK-0x400)
.equ _FIQ_STACK,		(_IRQ_STACK-0x400)
.equ _ABT_STACK,		(_FIQ_STACK-0x400)
.equ _UND_STACK,		(_ABT_STACK-0x400)
.equ _SYS_STACK,		(_UND_STACK-0x400)

.text
.code 32

__vector_undefined:
	nop
__vector_swi:
	ldr r0,[lr,#-4]
        mov  r1,#0x00FFFFFF
        and r0, r0, r1
        cps #0x1f
	bl syscall
__vector_prefetch_abort:	
	nop
__vector_data_abort:
	nop
__vector_reserved:
	nop
__vector_irq:
	// save registers
//	stmfd	r13!, {r0-r12,lr}
	// call interrupt handler
	bl	IRQ_handler
	// restore registers
//	ldmfd	r13!, {r0-r12,lr}
	// return address（IRQ lr-4)
//	subs	pc,lr, #4
__vector_fiq:
	nop

// IRQ enable set CPSR
	.global enable_IRQ
enable_IRQ:
	mrs	r0, cpsr
	bic r0, r0, #0x80
	msr cpsr_c,r0
	bx lr

// IRQ disable set CPSR
	.global disable_IRQ
disable_IRQ:
	mrs	r0, cpsr
	ldr	r1,	=0x80
	orr r0, r0, r1
	msr	cpsr_c, r0
	bx lr

.extern plat_boot
.extern __bss_start__
.extern __bss_end__

/*set stack*/
__vector_reset:
        msr cpsr_c,#(DISABLE_IRQ|DISABLE_FIQ|SVC_MOD)
	ldr	sp,=_SVC_STACK
	msr cpsr_c,#(DISABLE_IRQ|DISABLE_FIQ|IRQ_MOD)
	ldr	sp,=_IRQ_STACK
	msr cpsr_c,#(DISABLE_IRQ|DISABLE_FIQ|FIQ_MOD)
	ldr	sp,=_FIQ_STACK
	msr cpsr_c,#(DISABLE_IRQ|DISABLE_FIQ|ABT_MOD)
	ldr	sp,=_ABT_STACK
	msr cpsr_c,#(DISABLE_IRQ|DISABLE_FIQ|UND_MOD)
	ldr	sp,=_UND_STACK
	msr cpsr_c,#(DISABLE_IRQ|DISABLE_FIQ|SYS_MOD)
	ldr	sp,=_SYS_STACK

_clear_bss:
	ldr r1,_bss_start_
	ldr r3,_bss_end_
	mov r2,#0x0
1:
	cmp r1,r3
	beq _main
	str r2,[r1],#0x4
	b	1b
/*jump to plat_boot function in boot.c*/
_main:
	b plat_boot

_bss_start_:.word   __bss_start__
_bss_end_:.word   __bss_end__

.section .startup
.code 32
.align 0
_start:
	
	ldr	pc,_vector_reset
	ldr pc,_vector_undefined
	ldr pc,_vector_swi
	ldr pc,_vector_prefetch_abort
	ldr pc,_vector_data_abort
	ldr pc,_vector_reserved
	ldr pc,_vector_irq
	ldr pc,_vector_fiq

	.align 4

_vector_reset:	.word	__vector_reset
_vector_undefined:	.word	__vector_undefined
_vector_swi:	.word	__vector_swi
_vector_prefetch_abort:	.word	__vector_prefetch_abort
_vector_data_abort:	.word	__vector_data_abort
_vector_reserved:	.word	__vector_reserved
_vector_irq:	.word	__vector_irq
_vector_fiq:	.word	__vector_fiq 


