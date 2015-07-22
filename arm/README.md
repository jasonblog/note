# ARM

##2. ARM汇编语言

###2.1. ARM命名规则

ARM是Advanced RISC Machines的缩写，是典型的RISC(Reduced Instruction Set Computer)架构的CPU。ARM的版本控制的命名规则分成两类。 一类是基于ARM 架构的版本命名规则，它是一种架构设计的总和；另一类是基于某ARM架构版本的系列处理器的命名规则。

###2.1.1. ARM架构命名规则
```
| ARMv | n | variants | x（variants） |
```

ARM架构的命名由四部分组成：

- ARMv字符串，这部分固定不变。
- n，ARM指令集版本号，ARM架构版本发布了7个系列，所以n=[1:7]。其中最新的版本是第7版，Cortex系列CPU采用该版本。
- variants，变种，通常用大写字母来表示对一类指令或者指令集，变种就是这些大写字母的连写。
- x（variants），不支持x后指定的变种。

常见的变种有：

- T，Thumb指令集
- M ，长乘法指令
- E，增强型DSP指令
- J，Java虚拟机Jazelle指令集
- P，LDRD, MCRR, MRRC, PLD和STRD指令

例如，ARMv5TxP表示ARM指令集版本为5，支持Thumb指令集，不支持LDRD, MCRR, MRRC, PLD和STRD指令。

###表 1. ARM架构版本 版本	说明

<table summary="ARM架构版本" border="1"><colgroup><col><col></colgroup><thead><tr><th>版本</th><th>说明</th></tr></thead><tbody><tr><td>ARMv1<sup>[<a name="id3584976" href="#ftn.id3584976" class="footnote">a</a>]</sup></td><td>该版本的原型机是ARM1，没有用于商业产品</td></tr><tr><td>ARMv2</td><td>对v1版进行了扩展，包含了对32位结果的乘法指令和协处理器指令的支持。</td></tr><tr><td>ARMv3</td><td>ARM公司第一个微处理器ARM6核心是版本3的，它作为IP核、独立的处理器、具有片上高速缓存、MMU和写缓冲的集成CPU。
</td></tr><tr><td>ARMv4</td><td>应用非常广泛，但仅支持32位ARM指令集。</td></tr><tr><td>ARMv4T</td><td>同时支持ARM指令集4和Thumb指令集1。</td></tr><tr><td>ARMv5T</td><td>同时支持ARM指令集5和Thumb指令集2。添加计算开始0个数的clz指令和软中断bkpt指令</td></tr><tr><td>ARMv5TExP</td><td>在ARMv5T基础上添加增强型DSP指令，但不支持LDRD, MCRR, MRRC, PLD,
和STRD指令</td></tr><tr><td>ARMv5TE</td><td>在ARMv5T基础上添加增强型DSP指令。</td></tr><tr><td>ARMv5TEJ</td><td>在ARMv5TE基础上支持Jazelle指令集。</td></tr><tr><td>ARMv6</td><td>同时支持ARM指令集6和Thumb指令集3。增加和增强了相当多的单指令多数据处理指令。</td></tr><tr><td>ARMv6K</td><td>添加支持多处理的ARM指令集，以及一些额外的内存模型的特性。</td></tr><tr><td>ARMv6T2</td><td>推出Thumb-2技术。为16位和32位指令码的结合，免去了状态切换之复杂度。</td></tr><tr><td>ARMv7A</td><td>高性能运算，针对手机，PDA等手持便携设备。增加视频编解码和3D处理的NEON多媒体技术。</td></tr><tr><td>ARMv7R</td><td>减少输入输出延迟，提高指令预测精度，重视实时处理，针对网略设备，汽车仪表等应用。支持PMSA。</td></tr><tr><td>ARMv7M</td><td>低成本微处理器应用，去除NEON指令集。</td></tr></tbody><tbody class="footnotes"><tr><td colspan="2"><div class="footnote"><p><sup>[<a name="ftn.id3584976" href="#id3584976" class="para">a</a>] </sup>ARMv1至ARMv3版本已经废弃，不再被使用。</p></div></td></tr></tbody></table>


### 2.1.2. ARM处理器命名规则

ARM处理器的命名规则如下：
```
ARM{x}{y}{z}{T}{D}{M}{I}{E}{J}{F}{-S}
```

各组成部分解释如下：

x，处理器系列
y，存储管理/保护单元
z，cache
T，支持Thumb指令集
D，支持片上调试
M，支持快速乘法器
I，支持Embedded ICE，支持嵌入式跟踪调试
E，支持增强型DSP指令
J，支持Jazelle
F，具备向量浮点单元VFP
-S， 可综合版本
ARM使用一种基于数字的命名法。在早期（1990s），还在数字后面添加字母后缀，用来进一步明细该处理器支持的特性。就拿ARM7TDMI来说，T代表Thumb指令集，D是说支持JTAG调试(Debugging)，M意指快速乘法器，I则对应一个嵌入式ICE模块。后来，这4项基本功能成了任何新产品的标配，于是就不再使用这4个后缀相当于默许了。但是新的后缀不断加入，包括定义存储器接口的，定义高速缓存的，以及定义"紧耦合存储器（TCM）"的，于是形成了新一套命名法，这套命名法一直使用至今。比如ARM1176JZF-S，它实际上默认就支持TDMI功能，除此之外还支持JZF。

ARM7TDMI、ARM720T、ARM9TDMI、ARM940T、ARM920T、Intel的StrongARM等是基于ARMv4T版本。
ARM9E-S、ARM966E-S、ARM1020E、ARM1022E以及XScale是ARMv5TE的。
ARM9EJ-S、ARM926EJ-S、ARM7EJ-S、ARM1026EJ-S是基于ARMv5EJ的。ARM10也采用ARMv5TE。
ARM1136J(F)-S基于ARMv6主要特性有SIMD、Thumb、Jazelle、DBX、(VFP)、MMU。
ARM1156T2(F)-S基于ARMv6T2 主要特性有SIMD、Thumb-2、(VFP)、MPU。
ARM1176JZ(F)-S基于ARMv6KZ 在 ARM1136EJ(F)-S 基础上增加MMU、TrustZone。
ARM11MPCore基于ARMv6K 在ARM1136EJ(F)-S基础上可以包括1-4 核SMP、MMU。
从ARMv6指令集才开始支持SMP，这是由于从该指令集开始提供ldrex和strex系列独占(Exclusive)访问指令，该系列指令可以保证多CPU在同时访问内存时是互斥的。这也可以从内核代码中得到应征：
arch/arm/include/asm/atomic.h
#if __LINUX_ARM_ARCH__ >= 6
......
#else /* ARM_ARCH_6 */
#include <asm/system.h>

#ifdef CONFIG_SMP
#error SMP not supported on pre-ARMv6 CPUs
#endif
......
基于ARMv7架构的ARM处理器不再沿用过去的数字命名方式，而是冠以Cortex前缀，基于ARMv7A的处理器成为Cortex-A系列，基于ARMv7R的处理器成为Cortex-R系列，基于ARMv7M的处理器成为Cortex-M系列，

2.2. 程序状态寄存器

当前程序状态寄存器CPSR(Current Program Status Register)可以在任何工作模式下通过mrs指令访问。它包含了条件码(Condition code)标志位，禁止中断标志位，处理器工作模式位以及其他状态和控制信息。每一个中断模式还拥有一个单独的保存的程序状态寄存器SPSR(Saved Program Status Register)，当发生该模式的中断时，它被用来备份CPSR的值。用户模式和系统这两种模式不是中断模式，所以没有SPSR寄存器。在这两种模式下尝试访问SPSR结果未知。
图 1. CPSR和SPSR比特位

CPSR和SPSR比特位

如上图所示，程序状态寄存器比特位按访问权限分为四种：
Reserved bits: 保留位
User-writable bits: Usr模式可以更新的位：N,Z,C,V,Q,GE[3:0]和E.
Privileged bits: 特权模式才可更新的位：A,I,F和M[4:0]
Execution state bits: 执行状态位J和T：分别表示工作在指令集Jazilla和Thumb，如果J和T同为0，则工作在ARM指令集。不要通过MSR指令改变它，否则结果未知。只可在特权模式更改。
关于J和T的详细组合如下表所示：
表 2. Memory Hierarchy

J	T	指令集
0	0	ARM
0	1	Thumb
1	0	Jazelle
1	1	保留

2.3. ARM指令格式

ARM指令的基本格式如下：

<opcode>{<cond>}{S}    <Rd>, <Rn/#Num>{, <operand2>}
其中<>号内的项是必须的，{}号内的项是可选的，/表示任选其中之一。各项的说明如下：

opcode：指令助记符,	如mov，ldr等
cond：执行条件，如eq，ne等
S：是否影响CPSR寄存器的值
Rd：目标寄存器
Rn/#Num：第1个操作数的寄存器或者立即数
operand2：第2个操作数
灵活的使用第2个操作数“operand2”能够提高代码效率。它有如下的形式：

#immed_n——常数表达式，n表示立即数的位数，通常为12，也即最大为4096，例如：#0x1000。
Rm——寄存器方式；例如：r1
[Rm, shift]——寄存器移位方式；例如：[pc, #-4]
比如：subnes r1, r1, #0xd; 如果Z标志为0(不等)，则执行sub r1, r1, #0xd并根据结果更新CPSR寄存器。 参考资料：ARM嵌入式系统基础教程（第2版）PPT

指令中的执行条件由CPSR寄存器中的状态位控制，参考状态位。
表 3. 条件码表

条件码(二进制)	条件助记符	标志位	含义
0000	eq	Z==1	相等
0001	ne	Z==0	不等
0010	cs/hs	C==1	无符号数大于或等于
0011	cc/lo	C==0	无符号数小于
0100	mi	N==1	负数
0101	pl	N==0	正数或0
0110	vs	V==1	溢出
0111	vc	V==0	无溢出
1000	hi	C==1 且 Z==0	无符号数大于
1001	ls	C==0 且 Z==1	无符号数小于或等于
1010	ge	N==V	有符号数大于或等于
1011	lt	N!=V	有符号数小于
1100	gt	Z==0 且 N==V	有符号数大于
1101	le	Z==1 或 N!=V	有符号数小于或等于
1110	al	无条件	无条件执行(默认)

2.4. 测试用例

为了进行标志位等的验证，这里使用一个通用的示例程序，它包含两个文件：
main.S
.section .text
.align 2

.global main
main:
 mov r0, #12
 mov r1, #34
 mov r2, #56
 bl print
 mov r0, #0
 bl quit
注意到main.S中的子程序名为main，这是因为GCC编译器必须将main作为入口，否则无法使用Glibc的库函数。注意到mov命令，根据ATPS规则，这里是给print函数准备参数，这里可以更改r0和r1中的值，通过print来打印出它们。
print.c
#include <stdio.h>
#include <stdlib.h>

char regs[32][8] =
{
	"M0", "M1", "M2",	"M3",	"M4",	"T", "F",	"I", /* 0-7 */
	"A", "E", " ", " ", " ", "CV", " ", " ", /* 8-15 */
	"|", "G", "E", "|", " ", " ", " ", " ", /* 16-23 */
	"J", " ", " ", "Q", "V", "C", "Z", "N"  /* 24-31 */
};

void format(char *regname, int reg)
{
	int i = 0;

	if(!reg)
		return;

	printf("%3s", regname);
	for(i = 31; i >= 0; i--)
		printf("%3x", reg & (1 << i) ? 1 : 0);
	printf("\n");
}

void print(int r0, int r1, int r2)
{
	int i = 0;

	printf(" General out:\n");
  printf(" Hex\tr0 0x%08x\tr1 0x%08x\tr2 0x%08x\n", r0, r1, r2);
  printf(" Oct\tr0 %8d\tr1 %8x\tr2 %8d\n\n", r0, r1, r2);


	if(r0 || r1 || r2)
	{
		printf(" Format out:\n   ");
		for(i = 31; i >= 0; i--)
			printf("%3d", i);
		printf("\n   ");
		for(i = 31; i >= 0; i--)
			printf("%3s", regs[i]);
		printf("\n");
	}

	format("r0", r0);
	format("r1", r1);
	format("r2", r2);
}

void quit(int err)
{
	exit(err);
}
print.c中的print通过Glibc库函数printf打印出结果，而quit则通过标准库函数exit退出运行。另外编译命令和测试结果如下：
arm-linux-gcc  print.c  main.S -o test

# ./test
 General out:
 r0 0x0000000c  r1 0x00000022   r2 0x00000038

 Format out:
    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
     N  Z  C  V  O                                        CV                 I  F  T M4 M3 M2 M1 M0
 r0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  1  0  0
 r1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  1  0
 r2  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  1  1  0  0  0
借助c语言的格式化输出，能够很好的观察状态位的变化。
2.4.1. CPSR操作

修改main.S指令的如下，获取cpsr和spsr寄存器标志位：
main:
 mrs r0, cpsr
 mrs r1, spsr
 mov r2, #0
 ......
这里主要用到了mrs命令，它可以直接获取当前程序状态寄存器cpsr和保存的程序状态寄存器spsr，用户模式并没有spsr，所以它的值就是cpsr。以上程序结果如下所示：
 General out:
 r0 0x20000010  r1 0x20000010   r2 0x00000000

 Format out:
    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
     N  Z  C  V  O                                        CV                 I  F  T M4 M3 M2 M1 M0
 r0  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
 r1  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
可以看到r0和r1的值相同，并且只设置了C标志。而M模式位为0x10，为用户模式。通常有些情况下需要设置cpsr，可以通过msr指令实现。
 mov r0, #0
 msr cpsr_f, r0
 ......
注意这里的cpsr_f指明操作标志位[31:24]，这里将用户模式可以设置的位于[31:24]中的所有位清零。这里的f指明了传送的区域，它可以为以下几种值：
c：控制标志位[7:0]
x：扩展域标志位[15:8]
s：状态域标志位[23:16]
f: 条件域标志位[31:24]
2.4.2. 含s置位的指令

修改main.S指令的以测试含s置位指令：
main:
 mov r0, #0			/* without 's' */
 mrs r1, cpsr
 movs r0, #0    /* with s */
 mrs r2, cpsr
 bl print
 mov r0, #0
 bl quit
测试的结果如下所示：
 General out:
 r0 0x00000000  r1 0x20000010   r2 0x60000010

 Format out:
    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
     N  Z  C  V  O                                        CV                 I  F  T M4 M3 M2 M1 M0
 r1  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
 r2  0  1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
可以看到第一条指令并没有影响标志位，而movs指令则改变了Z标志位，也即当前的运算结果为0。
2.4.3. Z标志条件编码

首先通过movs赋值r1为0，此时Z标志位1，r1记录此时的cpsr值。然后改变r2的值为0，以实现测试moveq和movne的测试。
.macro  testz, cmd
 movs r0, #0
 mrs r1, cpsr
 mov r2, #0
 \cmd r2, #1
 bl print
.endm

.global main
main:
 testz moveq
 testz movne
 mov r0, #0
 bl quit
moveq当Z==1时才被执行，显然此时被执行，r2的值被赋值为1。接着测试movne，它只有在Z==0时才被执行，所以这里r2的值并没有被赋值为1，而依然是0。
 General out:
 r0 0x00000000  r1 0x60000010   r2 0x00000001

 Format out:
    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
     N  Z  C  V  O                                        CV                 I  F  T M4 M3 M2 M1 M0
 r1  0  1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
 r2  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1
 General out:
 r0 0x00000000  r1 0x60000010   r2 0x00000000

 Format out:
    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
     N  Z  C  V  O                                        CV                 I  F  T M4 M3 M2 M1 M0
 r1  0  1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
其他标志位可以使用相同方法进行测试。
2.5. 原码和补码

对于计算机而言，最根本的动作就是高低电位的跳变，从而表示0和1两种状态。在计算机中，所有的负数都用补码表示，而负数的原码对于计算机来说没有任何意义。对于人来说，通常使用十进制，十进制在表示负数时有符号-来表示。但是对于针对计算机而设计的二进制来说，并没有所谓的符号-来表示负数，而是要拿出一位来表示数的正负——最高位。考虑以下的代码：
int main()
{
        int tmp = -0x1;
        printf("tmp:%x\n", tmp);
        return 0;
}
尽管编译时没有错误，但是通常人们并不使用针对计算机设计的二，八或者十六进制添加符号-来表示负数，而是使用十进制-1，或者直接指定符号位：0x80000001。但是注意通过二，八或者十六进制表示的没有符号-的负数，永远都被直接编译进指令中作为补码使用，否则编译器将把它们转换为补码后再放入指令中。所以0x80000001并不表示-1，而是-2147483647这样一个数字，这是因为0x80000001是它的补码，而非-1的。看来补码的引入并没有符合人的使用习惯，那么引入补码的意义是什么呢？
模的概念：把一个计量单位称之为模或模数。例如，时钟是以12进制进行计数循环的，即以12为模。在时钟上，时针加上（正拨）12的整数位或减去（反拨）12的整数位，时针的位置不变。14点钟在舍去模12后，成为（下午）2点钟（14=14-12=2）。从0点出发逆时针拨10格即减去10小时，也可看成从0点出发顺时针拨2格（加上2小时），即2点（0-10=-10=-10+12=2）。因此，在模12的前提下，-10可映射为+2。由此可见，对于一个模数为12的循环系统来说，加2和减10的效果是一样的；因此，在以12为模的系统中，凡是减10的运算都可以用加2来代替，这就把减法问题转化成加法问题了（注：计算机的硬件结构中只有加法器，所以大部分的运算都必须最终转换为加法）。10和2对模12而言互为补数。

同理，计算机的运算部件与寄存器都有一定字长的限制（假设字长为8），因此它的运算也是一种模运算。当计数器计满8位也就是256个数后会产生溢出，又从头开始计数。产生溢出的量就是计数器的模，显然，8位二进制数，它的模数为28=256。在计算中，两个互补的数称为"补码"。 计算机引入补码后：

可以方便地将减法运算转化成加法运算，运算过程得到简化。正数的补码即是它所表示的数的真值，而负数的补码的数值部份却不是它所表示的数的真值。采用补码进行运算，所得结果仍为补码。
0的补码为0b00000000。但是符号位带来了负0值，它的补码与原码相同为0b10000000，所表示的值为-2N - 1 。对于8位数来说，为-128。
若字长为8位，则补码所表示的范围为-128～+127；进行补码运算时，应注意所得结果不应超过补码所能表示数的范围。
图 2. 原码和补码的关系

原码和补码的关系

2.6. 条件码标志位

N(Negative)、Z(Zero)、C(Carry)及V(oVerflow)统称为条件标志位。大部分的ARM 指令可以根据CPSR 中的这些条件标志位来选择性地执行。它们在以下两种情况下被改变：
比较指令:CMN,CMP,TEQ和TST
一些数学运算指令，逻辑和数据传输指令有两种变体：标志保持；标志改变，其中标志改变的指令后缀为S。有些指令没有标志改变的变体。标志改变变体指令通常只有在目标寄存器不为R15(pc)时才会改变标志位。
在任何一种情况下，这些标志位发生了改变，这意味着：
N：它被设置为结果的bit[31]位，当两个补码表示的有符号整数运算时，N=1 表示运算的结果为负数；N=0 表示结果为正数或零。
Z：Z=0 表示运算的结果不为零。对于CMP 指令，Z=1表示进行比较的两个数大小相等。
C：分4 种情况讨论C的设置方法：
在加法指令中(包括比较指令CMN)，当结果产生了进位，则C=1，表示无符号数运算发生上溢出；其他情况下C=0。
在减法指令中(包括比较指令CMP)，当运算中发生借位则C=0表示无符号数运算发生下溢出；其他情况下C=1。
对于包含移位操作的非加／减法运算指令，C中包含最后一次溢出的位数数值。
对于其他非加／减法运算指令，C位的值通常不受影响。
V：对于加／减法运算指令，当操作数和运算结果为二进制的补码表示的带符号数时V＝1 表示符号位溢出。通常其他的指令不影响V 位，具体可参考各指令的说明。
通过msr指令直接改变CPSR/SPSR标志位。
mrc读协处理器指令，当目的寄存器为r15(pc)时，用来将协处理器中的条件标志位拷贝到ARM处理器中。
ldm系列指令，通常用来拷贝SPSR到CPSR，用于中断返回。
rfe指令在特权模式下从内存恢复CPSR。
数学和逻辑逻辑指令在目的寄存器为r15时，可能拷贝SPSR到CPSR中。可以用于从中断中返回。
2.6.1. N负数标志位测试

测试N标志位的思想很简单，就是根据指令结果的符号位为0和1来查看N标志位。
main:
 /* save orig cpsr->r0 */
 mrs r0, cpsr

 /* give a non-Neg value get cpsr->r1 */
 movs r3, #1
 mrs r1, cpsr

 /* give a Neg value get cpsr->r2 */
 movs r3, #-1
 mrs r2, cpsr

 ......
输出结果如下所示，显然r1的N标志位为0，而r2则变为了1。
 General out:
 Hex    r0 0x20000010   r1 0x20000010   r2 0xa0000010
 Oct    r0 536870928    r1 20000010     r2 -1610612720

 Format out:
    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
     N  Z  C  V  Q        J              |  E  G  |       CV           E  A  I  F  T M4 M3 M2 M1 M0
 r0  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
 r1  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
 r2  1  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
2.6.2. Z零标志位测试

测试N标志位的思想很简单，就是根据指令结果的是否为0来查看Z标志位。
main:
 /* save orig cpsr->r0 */
 mrs r0, cpsr

 /* give a non-Zero let cpsr->r1 */
 movs r3, #1
 mrs r1, cpsr

 /* give a Zero let cpsr->r2 */
 movs r3, #0
 mrs r2, cpsr

 ......
输出结果如下所示，显然r1的Z标志位为0，而r2则变为了1。
 General out:
 Hex    r0 0x20000010   r1 0x20000010   r2 0x60000010
 Oct    r0 536870928    r1 20000010     r2 1610612752

 Format out:
    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
     N  Z  C  V  Q        J              |  E  G  |       CV           E  A  I  F  T M4 M3 M2 M1 M0
 r0  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
 r1  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
 r2  0  1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
2.6.3. C进位标志位测试

测试C标志位相对复杂，包括四种情况。
2.6.3.1. 加法指令进位

加法指令add将所有操作数当做无符号处理，CPU内部使用CarryFrom(Rn + shifter_operand)来计算是否进位，其中rn和shifter_operand分别为第一操作数和第二操作数。测试思想为给第一操作数赋值为0xfffffffe，这保证在第一次加1后，不会进位，再次加1后必定进位。另外注意到r0存数了0xfffffffe + 2的结果。CarryFrom的算法很简单，在ARM指令集中它通过比较Rn + shifter_operand > 232-1的值决定是否上溢。对于8位和16位的语出操作记作CarryFrom8和CarryFrom16，它们通常被用在字节和半字操作中[1]。
main:
 /* clean all condition flags */
 msr cpsr_f, #0

 /* make sure the add result won't carry and
  let cpsr->r1 */
 mov r3, #0xfffffffe
 adds r3, r3, #1
 mrs r1, cpsr

 /* carried and let cpsr->r2 */
 adds r3, r3, #1
 mov r0, r3

 ......
输出结果如下所示，显然r1的C标志位为0，而由于出现进位r2则变为了1。
 Hex    r0 0x00000000   r1 0x80000010   r2 0xbeb62ebc
 Oct    r0        0     r1 80000010     r2 -1095356740

 Format out:
    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
     N  Z  C  V  Q        J              |  E  G  |       CV           E  A  I  F  T M4 M3 M2 M1 M0
 r1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
 r2  1  0  1  1  1  1  1  0  1  0  1  1  0  1  1  0  0  0  1  0  1  1  1  0  1  0  1  1  1  1  0  0
2.6.3.2. 减法指令借位

减法指令add将所有操作数当做无符号处理，CPU内部使用NOT BorrowFrom(Rn - shifter_operand)来计算是否借位，其中rn和shifter_operand分别为第一操作数和第二操作数。BorrowFrom的算法很简单，在ARM指令集中它通过比较Rn - shifter_operand < 0的值决定是否借位。如果发生借位，那么C标志为0，否则为1。

 /* clean all condition flags */
 msr cpsr_f, #0

 /* make sure the sub result won't borrow and
  let cpsr->r1 */
 mov r3, #0x1
 subs r3, r3, #1
 mrs r1, cpsr

 /* borrowed let cpsr->r2 */
 subs r3, r3, #1
 mrs r2, cpsr

 mov r0, r3
 ......
测试原理：将作出的被减数设置为1，第一次减1时，不发生借位置C为1，此时r3内容为0，然后再减去1，显然发生借位，所以置C为1。
 General out:
 Hex    r0 0xffffffff   r1 0x60000010   r2 0x80000010
 Oct    r0       -1     r1 60000010     r2 -2147483632

 Format out:
    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
     N  Z  C  V  Q        J              |  E  G  |       CV           E  A  I  F  T M4 M3 M2 M1 M0
 r0  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1
 r1  0  1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
 r2  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
注意r0中为1-2的值，也即为-1的补码。
2.6.3.3. 移位指令

asr：算术右移指令，将符号位拷贝到空位，若移位量为32，则目标寄存器清0，且最后移除的位保留在C中；若移位量大于32，则目标寄存器和C均被清0。
lsl：逻辑左移指令，空位补0，若移位量为32，则目标寄存器清0，且最后移出的位保留在C中；若移位量大于32，则目标寄存器和C均被清0。若位移量为0，不影响C标志。
lsr：逻辑右移指令，空位补0，若移位量为32，则目标寄存器清0，且最后移出的位保留在C中；若移位量大于32，则目标寄存器和C均被清0。若位移量为0，不影响C标志。
ror：数据循环右移指令，影响C标志。
2.6.3.4. V溢出位

在加减运算中，通过OverflowFrom(Rn + shifter_operand)来判断是否溢出。OverflowFrom分为两种情况：加法运算中，如果操作数的符号位相同，但是结果的符号位发生改变；在减法运算中，如果两操作数符号位不同，结果与第一个操作数的符号位也不同，则溢出，也即超出补码表示范围。
main:
 /* clean all condition flags */
 msr cpsr_f, #0

 /* make sure the adds result won't overflow */
 mov r3, #0x6ffffffe
 adds r3, r3, #1
 mrs r1, cpsr

 /* overflowed let cpsr->r2 */
 adds r3, r3, #1
 mrs r2, cpsr

 mov r0, r3
 ......
这里以加法为例，首先设置r3为0x7ffffffe，确保第一次加1时，不会溢出，而第二次则会发生结果符号位的改变。
 General out:
 Hex    r0 0x80000000   r1 0x00000010   r2 0x90000010
 Oct    r0 -2147483648  r1       10     r2 -1879048176

 Format out:
    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
     N  Z  C  V  Q        J              |  E  G  |       CV           E  A  I  F  T M4 M3 M2 M1 M0
 r0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 r1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
 r2  1  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
显然r1中的V为0，而r2中的V为1。
2.7. 跳转指令

2.7.1. b和bl指令

b(Branch)和bl(Branch and Link)指令实现分枝跳转，并且支持条件跳转。bl指令与b指令的区别是它在跳转前会将当前pc的下一条指令地址存入lr寄存器，指令编码的区别就在于L位是否为1。所以bl适合子程序调用。bl的指令格式如下，signed_immed_24表示24位有符号立即数，显然它可以表示的大小为+-8M，由于ARM的指令长度总是32bits，所以可以表示+/-32MB的地址空间。
31   28 27 26 25 24
+-----+--------+---+--------------------------------+
|cond | 1  0  1| L |        signed_immed_24         |
+-----+--------+---+--------------------------------+
bl指令的伪代码表示如下：
if ConditionPassed(cond) then
	if L == 1 then
		LR = address of the instruction after the branch instruction
PC = PC + (SignExtend_30(signed_immed_24) << 2)
bl指令格式如下，它的参数是子程序的标签，并不接受立即数，所以这牵涉到signed_immed_24计算方法。显然，它是在编译器编译过程中确定。
bl{cond} label
首先确定bl指令的地址。
然后确定bl参数label代表的地址。
计算label与bl指令之间的偏移offset，显然该偏移的单位为字节。
根据公式(offset - 8) >> 2 计算指令偏移数，这里就是signed_immed_24。
如果得到的signed_immed_24值不在−33554432 和 33554428范围内，则编译器报错。
offset之所以要减去8，是因为PC + (SignExtend_30(signed_immed_24) << 2)中的pc是指当前正在执行的pc(excute)，而pc(excute)= pc(fetch) + 8。指令中的label对应的地址相当于pc(fetch)。这里用一个简单的例子说明：
.section .text
.align 2

.global main
main:
 mov r0, #12
 mov r1, #34
 mov r2, #56
 bl over
.global over
over:
 mov r0, #0
以上是一个简单的跳转测试，over标签与bl指令地址相差1个指令，4个字节。反汇编代码如下：
00008790 <main>:
    8790:       e3a0000c        mov     r0, #12 ; 0xc
    8794:       e3a01022        mov     r1, #34 ; 0x22
    8798:       e3a02038        mov     r2, #56 ; 0x38
    879c:       ebffffff        bl      87a0 <over>

000087a0 <over>:
    87a0:       e3a00000        mov     r0, #0  ; 0x0
ebffffff就是指令码，eb无需解释了，而0xffffff就是signed_immed_24，它通过(0x87a0 - 0x879c - 8) >> 2计算而得，这个值就是-1，由于指令中的操作数都是采用补码表示，所以0xffffff就是-1的补码。修改例子如下，此时地址相差3个指令，这保证偏移的指令数为正整数1。
.section .text
.align 2

.global main
main:
 mov r0, #12
 bl over
 mov r1, #34
 mov r2, #56
.global over
over:
 mov r0, #0
观察反汇编的结果，显然指令码为eb000001，正数1的补码和原码相同。
00008790 <main>:
    8790:       e3a0000c        mov     r0, #12 ; 0xc
    8794:       eb000001        bl      87a0 <over>
    8798:       e3a01022        mov     r1, #34 ; 0x22
    879c:       e3a02038        mov     r2, #56 ; 0x38

000087a0 <over>:
    87a0:       e3a00000        mov     r0, #0  ; 0x0
图 3. bl跳转示意图

bl跳转示意图

b指令的存在意义在于，有时候无需返回，或者lr寄存器另作它用，此时如需返回需要显式保存下一条指令地址到其他寄存器。
2.7.2. ldr和adr指令

ldr和adr是另两个常用来实现跳转的指令，严格来说ldr指令有两种，根据操作数，它可以是指令，也可以是伪指令。 作为指令的ldr的语法格式如下，通常实现把一个32位的字从内存装入一个寄存器。
ldr{<cond>} <Rd>，<addr_mode>
指令编码格式如下：
31   28 27 26 25242322212019 1615 1211              0
+-----+------+-+-+-+-+-+-+----+----+----------------+
|cond | 0  1 |1|P|U|0|W|1| Rn | Rd |  addr_mode     |
+-----+------+-+-+-+-+-+-+----+----+----------------+
ldr指令根据addr_mode所确定的地址模式将一个32位字读取到指令中的目标寄存器Rd。如果指令中的寻址方式确定的地址不是字对齐的，则读出的数值要进行循环右移。所移位数为寻址方式确定的地址bits[1:0]的8倍，也就是说处理器将取到的数值作为字的最低位处理。
addr_mode它确定了指令编码中的I、P、U、W、Rn和addr_mode位。所有的寻址模式中，都会 确定一个基址寄存器Rn，通常为pc。考虑如下的ldr测试程序：

......
.global main
main:
 mov r0, pc
 ldr r1, [r0]
 ldr r2, main
 ldr r0, =main
 bl print
 b quit
编译后的反汇编程序如下，显然对于第一个还有第二个指令来说，它们均是作为指令编译的，并且是相对于pc(当前正在取指的指令地址，而非正在执行的当前ldr指令的地址)寄存器进行偏移。而第三天指令则是将main的绝对地址0x00008790装载到r0。所以可以通过ldr指令进行相对偏移来跳转到C函数
00008790 <main>:
    8790:       e1a0000f        mov     r0, pc
    8794:       e5901000        ldr     r1, [r0]
    8798:       e51f2010        ldr     r2, [pc, #-16]  ; 8790 <main>
    879c:       e59f0004        ldr     r0, [pc, #4]    ; 87a8 <main+0x18>
    87a0:       ebffff9d        bl      861c <print>
    87a4:       eafffff2        b       8774 <quit>
    87a8:       00008790        .word   0x00008790
2.8. 协处理器指令

2.9. ARM汇编伪指令

2.9.1. .macro

宏指令是由.macro和.endm来定义开始和结束的一组指令的集合。它类似于C语言中的宏函数，使用它将可以：

使代码可以重用，减小代码量。
使程序逻辑更加清晰
伪指令格式：

.macro macroname {$param1} {$param2} ...
@commands
.mend
macroname是定义的宏名，$paramx是宏指令的参数，当宏指令被展开式将被替换成相应的值，类似于函数中的形式参数。一个例子是Linux内核中用于打印信息的kputc宏命令。在使用宏参数时必须这样“\$param”表示参数。

.macro  kphex,val,len
mov     r0, \val
mov     r1, #\len
bl      phex
.endm
在C语言和汇编语言的交互调用中，使用宏可以简化参数的传递。可以使用.if宏开关来定义宏指令，.exitm可以跳出宏。一个进行移位运算的例子如下：

.macro shiftleft reg, shift
.if \shift < 0
mov \reg, \reg, asr #-\shift
.exitm
.endif
mov \reg, \reg, lsl #\shift
.endm
2.9.2. .rept

.rept 用于重复执行一组指令，它的格式如下：

.rept <repeat>
.endr
repeat指明重复执行的次数。一个例子是Linux内核中用于nop的操作：

.rept 8
mov	  r0, r0
.endr
2.10. Sandbox

表 4. Memory Hierarchy




图 4. 内核RAM布局

内核RAM布局



[1] 尽管ARM提供了丰富的相关指令，比如uadd8,uadd16等，但并不是所有编译器都能够支持这些指令。

上一页 	 	 下一页
Linux内核学习和研究及嵌入式(ARM)学习和研究的开放文档 	起始页	 3. ARM寻址方式
