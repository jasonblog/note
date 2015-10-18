#
# 
#   reference: 
#   david leels <davidontech@gmail.com>
#   http://stackoverflow.com/questions/6870712/beagleboard-bare-metal-programming
#

#UART3 THR_REG register address
.equ UART3_THR_REG, 0x20201000

.arm

_start:
    ldr r0,=UART3_THR_REG
    adr	r1,.L0
	bl helloworld
.L1:
	b	.L1

.align 2
.L0:	
	.ascii	"helloworld\n\0"
