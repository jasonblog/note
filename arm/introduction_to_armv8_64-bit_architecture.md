# Introduction to ARMv8 64-bit Architecture

##Introduction

The ARM architecture is a Reduced Instruction Set Computer (RISC) architecture, indeed its originally stood for “Acorn RISC Machine” but now stood for “Advanced RISC Machines”.
In the last years, ARM processors, with the diffusion of smartphones and tablets, are beginning very popular: mostly this is due to reduced costs, and a more power efficiency compared to other architectures as CISC:



RISC architectures (ARM, Mips, …) peculiarity:

- The load/store architecture only allows memory to be accessed by load and store operations, and all values for an operation need to be loaded from memory and be present in registers, so operations as “add reg,[address]” are not permitted!

- Another difference with CISC architectures: when a Branch and Link is called (in Intel arch. is the “call” operation) the return address is stored in a special register and not in the stack.



A lack into ARM architecture is the absence of multi-threading support, which is present in many others architectures as: Intel and Mips.
Cause of AArch32 (32bit) is most documented: Arm on wiki, Cambridge University – Operation System Development I decided to talk only about AArch64 (64bit).


## Processors:

A short ARM processors list:

- Classic or Cortext-A: with DSP, Floating Point, TrustZone e Jazelle extensions. ARMv5 e ARM6 (2001)
- Cortex-M: ARM Thumb®-2 technology which provides excellent code density. With Thumb-2 technology, the Cortex-M processors support a fundamental base of 16-bit Thumb instructions, extended to include more powerful 32-bit instructions. First Multi-core. (2004)
- Cortex-R: ARMv7 Deeply pipelined micro-architecture,Flexible Multi-Processor Core (MPCore) configurations:Symmetric Multi-Processing (SMP) & Asymmetric Multi-Processing (AMP), LPAE extension.
- Cortex-A50: ARMv8-A 64bit with load-acquire and store-release features , which are an excellent match for the C++11, C11 and Java memory models. (2011)

##Extensions

With every new version of ARM, there’re new extensions provided, the v8 architecture has these:


- Jazelle is a Java hardware/software accelerator: “ARM Jazelle DBX (Direct Bytecode eXecution) technology for direct bytecode execution of Java”. On Sofware side: Jazelle MobileVM is a complete JVM which is Multi-tasking, engineered to provide high performance multi-tasking in a very small memory footprint

- Floating Point: for floating point operations

- NEON: the ARM SIMD 128 bit (Single instruction, multiple data) engine and DSP the SIMD 32bit engine useful to make linear algebra operations

- Cryptographic Extension is an extension of the SIMD support and operates on the vector register file. It provides instructions for the acceleration of encryption and decryption to support the following: AES, SHA1, SHA2-256.

- TrustZone: is a system-wide approach to security for a wide array of client and server computing platforms include payment protection technology, digital rights management, BYOD, and a host of secured enterprise solutions

- Virtualization Extensions with the Large Physical Address Extension (LPAE) enable the efficient implementation of virtual machine hypervisors for ARM architecture compliant processors.
    - The visualization extensions provide the basis for ARM architecture compliant processors to address the needs of both client and server devices for the partitioning and management of complex software environments into virtual machines.
    - The Large Physical Address extension provides the means for each of the software environments to utilize efficiently the available physical memory when handling large amounts of data
    

![](./images/V5_to_V8_Architecture.jpg)


##Architectures

- AArch64 the ARMv8-A 64-bit execution state, that uses 31 64-bit general purpose registers (R0-R30), and a 64-bit program counter (PC), stack pointer (SP), and exception link registers(ELR). Provides 32 128-bit registers for SIMD vector and scalar floating-point support (V0-V31).

- A64 instructions have a fixed length of 32 bits and are always little-endian.
AArch32 is the ARMv8-A 32-bit execution state, that uses 13 32-bit general purpose registers (R0-R12), a 32-bit program counter (PC), stack pointer (SP), and link register (LR). Provides 32 64-bit registers for Advanced SIMD vector and scalar floating-point support.
AArch32 execution state provides a choice of two instruction sets, A32 (ARM) and T32 (Thumb2). Operation in AArch32 state is compatible with ARMv7-A operation.

- T32: 16-bit instructions are decompressed transparently to full 32-bit ARM instructions in real time without performance loss.Thumb-2 technology made Thumb a mixed (32- and 16-bit) length instruction set

##Data types

- Data types are simply these:
- Byte: 8 bits.
- Halfword: 16 bits.
- Word: 32 bits.
- Doubleword: 64 bits.
- Quadword: 128 bits.


The architecture also supports the following floating-point data types:

- Half-precision floating-point formats.
- Single-precision floating-point format.
- Double-precision floating-point format.

In this short guide, I don’t talk about floating point assembly instructions to don’t make it too long, if you want know more about, you can see the ARM Architecture Reference Manual.

## Exception levels

UIC Academy

Search UIC...
Community
Downloads
Library
About Us
You are in: UIC > Development > Introduction to ARMv8 64-bit Architecture
Introduction to ARMv8 64-bit Architecture
april 9, 2014 by pnuic
Introduction

The ARM architecture is a Reduced Instruction Set Computer (RISC) architecture, indeed its originally stood for “Acorn RISC Machine” but now stood for “Advanced RISC Machines”.
In the last years, ARM processors, with the diffusion of smartphones and tablets, are beginning very popular: mostly this is due to reduced costs, and a more power efficiency compared to other architectures as CISC:

Complex Instruction Set Computer (CISC) processors, like the x86, have a rich instruction set capable of doing complex things with a single instruction. Such processors often have significant amounts of internal logic that decode machine instructions to sequences of internal operations (microcode).RISC architectures, in contrast, have a smaller number of more general purpose instructions, that might be executed with significantly fewer transistors, making the silicon cheaper and more power efficient. Like other RISC architectures, ARM cores have a large number of general-purpose registers and many instructions execute in a single cycle. It has simple addressing modes, where all load/store addresses can be determined from register contents and instruction fields.

RISC architectures (ARM, Mips, …) peculiarity:

The load/store architecture only allows memory to be accessed by load and store operations, and all values for an operation need to be loaded from memory and be present in registers, so operations as “add reg,[address]” are not permitted!
Another difference with CISC architectures: when a Branch and Link is called (in Intel arch. is the “call” operation) the return address is stored in a special register and not in the stack.
A lack into ARM architecture is the absence of multi-threading support, which is present in many others architectures as: Intel and Mips.
Cause of AArch32 (32bit) is most documented: Arm on wiki, Cambridge University – Operation System Development I decided to talk only about AArch64 (64bit).

Processors:

A short ARM processors list:

Classic or Cortext-A: with DSP, Floating Point, TrustZone e Jazelle extensions. ARMv5 e ARM6 (2001)
Cortex-M: ARM Thumb®-2 technology which provides excellent code density. With Thumb-2 technology, the Cortex-M processors support a fundamental base of 16-bit Thumb instructions, extended to include more powerful 32-bit instructions. First Multi-core. (2004)
Cortex-R: ARMv7 Deeply pipelined micro-architecture,Flexible Multi-Processor Core (MPCore) configurations:Symmetric Multi-Processing (SMP) & Asymmetric Multi-Processing (AMP), LPAE extension.
Cortex-A50: ARMv8-A 64bit with load-acquire and store-release features , which are an excellent match for the C++11, C11 and Java memory models. (2011)
Extensions

With every new version of ARM, there’re new extensions provided, the v8 architecture has these:

Jazelle is a Java hardware/software accelerator: “ARM Jazelle DBX (Direct Bytecode eXecution) technology for direct bytecode execution of Java”. On Sofware side: Jazelle MobileVM is a complete JVM which is Multi-tasking, engineered to provide high performance multi-tasking in a very small memory footprint
Floating Point: for floating point operations
NEON: the ARM SIMD 128 bit (Single instruction, multiple data) engine and DSP the SIMD 32bit engine useful to make linear algebra operations
Cryptographic Extension is an extension of the SIMD support and operates on the vector register file. It provides instructions for the acceleration of encryption and decryption to support the following: AES, SHA1, SHA2-256.
TrustZone: is a system-wide approach to security for a wide array of client and server computing platforms include payment protection technology, digital rights management, BYOD, and a host of secured enterprise solutions
Virtualization Extensions with the Large Physical Address Extension (LPAE) enable the efficient implementation of virtual machine hypervisors for ARM architecture compliant processors.
The visualization extensions provide the basis for ARM architecture compliant processors to address the needs of both client and server devices for the partitioning and management of complex software environments into virtual machines.
The Large Physical Address extension provides the means for each of the software environments to utilize efficiently the available physical memory when handling large amounts of data
V5_to_V8_Architecture

Architectures

AArch64 the ARMv8-A 64-bit execution state, that uses 31 64-bit general purpose registers (R0-R30), and a 64-bit program counter (PC), stack pointer (SP), and exception link registers(ELR). Provides 32 128-bit registers for SIMD vector and scalar floating-point support (V0-V31).
A64 instructions have a fixed length of 32 bits and are always little-endian.
AArch32 is the ARMv8-A 32-bit execution state, that uses 13 32-bit general purpose registers (R0-R12), a 32-bit program counter (PC), stack pointer (SP), and link register (LR). Provides 32 64-bit registers for Advanced SIMD vector and scalar floating-point support.
AArch32 execution state provides a choice of two instruction sets, A32 (ARM) and T32 (Thumb2). Operation in AArch32 state is compatible with ARMv7-A operation.
T32: 16-bit instructions are decompressed transparently to full 32-bit ARM instructions in real time without performance loss.Thumb-2 technology made Thumb a mixed (32- and 16-bit) length instruction set
Data types

Data types are simply these:

Byte: 8 bits.
Halfword: 16 bits.
Word: 32 bits.
Doubleword: 64 bits.
Quadword: 128 bits.
The architecture also supports the following floating-point data types:

Half-precision floating-point formats.
Single-precision floating-point format.
Double-precision floating-point format.
In this short guide, I don’t talk about floating point assembly instructions to don’t make it too long, if you want know more about, you can see the ARM Architecture Reference Manual.

Exception levels

There’re four exception levels, which replaces the 8 different processor modes, they work as the ring in Intel architectures, they are a form of privilege hierarchy:

- EL0 is the least privileged level, indeed it is called unprivileged execution. Apps are runned here.

- EL1: here can be runned OS kernel

- EL2: provides support for virtualization of Non-secure operation. Hypervisor can runned here.

- EL3 provides support for switching between two Security states, Secure state and Non-secure state. Secure monitor can be runned here.

When executing in AArch64 state, execution can move between Exception levels only on taking an exception or on returning from an exception.
Each of the 4 privilege levels has 3 private banked registers: the Exception Link Register, Stack Pointer and Saved PSR.


##Interprocessing: AArch64 <=> AArch32

Interprocessing is the term used to describe moving between the AArch64 and AArch32 Execution states.
The Execution state can change only on a change of Exception level. This means that the Execution state can change only on taking an exception to a higher Exception level, or returning from an exception to a lower Exception level.
On taking an exception to a higher Exception level, the Execution state either:

- Remains unchanged.
- Changes from AArch32 state to AArch64 state.

On returning from an exception to a lower Exception level, the Execution state either:

- Remains unchanged.
- Changes from AArch64 state to AArch32 state.

##The A64 Register

A64 has 31 general-purpose registers (integer) more the zero register and the current stack pointer register, here all the registers:

<table border="1">
<tbody>
<tr>
<td><strong>W<em>n</em></strong></td>
<td>32 bits</td>
<td>General-purpose register: n can be 0-30</td>
</tr>
<tr>
<td><strong>X<em>n</em></strong></td>
<td>64 bits</td>
<td>General-purpose register: n can be 0-30</td>
</tr>
<tr>
<td><strong>WZR</strong></td>
<td>32 bits</td>
<td>Zero register</td>
</tr>
<tr>
<td><strong>XZR</strong></td>
<td>64 bits</td>
<td>Zero register</td>
</tr>
<tr>
<td><strong>WSP</strong></td>
<td>32 bits</td>
<td>Current stack pointer</td>
</tr>
<tr>
<td><strong>SP</strong></td>
<td>64 bits</td>
<td>Current stack pointer</td>
</tr>
</tbody>
</table>

![](./images/reg.jpg)

How registers should be using by compilers and programmers:

- r30 (LR): The Link Register, is used as the subroutine link register (LR) and stores the return address when Branch with Link operations are performed.
- r29 (FP): The Frame Pointer
- r19…r28: Callee-saved registers
- r18: The Platform Register, if needed; otherwise a temporary register.
- r17 (IP1): The second intra-procedure-call temporary register (can be used by call veneers and PLT code); at other times may be used as a temporary register.
- r16 (IP0): The first intra-procedure-call scratch register (can be used by call veneers and PLT code); at other times may be used as a temporary register.
- r9…r15: Temporary registers
- r8: Indirect result location register
- r0…r7: Parameter/result registers

The PC (program counter) has a limited access, only few instructions, as BL and ADL, can modify it.

## The use of Stack

The stack implementation is full-descending: in a push the stack pointer is decremented, i.e the stack grows towards lower address.
Another features is that stack must be quad-word aligned: SP mod 16 = 0.

A64 instructions can use the stack pointer only in a limited number of cases:

- Load/Store instructions use the current stack pointer as the base address: When stack alignment checking is enabled by system software and the base register is SP, the current stack pointer must be initially quadword aligned, That is, it must be aligned to 16 bytes. Misalignment generates a Stack Alignment fault.

- Add and subtract data processing instructions in their immediate and extended register forms, use the current stack pointer as a source register or the destination register or both.

- Logical data processing instructions in their immediate form use the current stack pointer as the destination register.

## Process State

PSTATE (process state, CPSR on AArch32) holds process state related information, his flags will be change with compare instructions, for example, so it is used by processor to see if make a branch (jump in Intel terminology) or not.

<table border="1">
<tbody>
<tr>
<td><em><strong>N,</strong></em><br>
<em><strong> Z,</strong></em><br>
<em><strong> C,</strong></em><br>
<em><strong> V,</strong></em><br>
<em><strong> D,</strong></em><br>
<em><strong> A,</strong></em><br>
<em><strong> I,</strong></em><br>
<em><strong> F,</strong></em><br>
<em><strong> SS,</strong></em><br>
<em><strong> IL,</strong></em><br>
<em><strong> EL,</strong></em><br>
<em><strong> nRW,</strong></em><br>
<em><strong> SP,</strong></em><br>
<em><strong> Q,</strong></em><br>
<em><strong> GE,</strong></em><br>
<em><strong> IT,</strong></em><br>
<em><strong> J,</strong></em><br>
<em><strong> T,</strong></em><br>
<em><strong> E,</strong></em><br>
<em><strong> M</strong></em></td>
<td>Negative condition flag<br>
Zero condition flag<br>
Carry condition flag<br>
oVerflow condition flag<br>
Debug mask bit<span style="color: #008000;"> <em>[AArch64 only]</em></span><br>
Asynchronous abort mask bit<br>
IRQ mask bit<br>
FIQ mask bit<br>
Software step bit<br>
Illegal execution state bit<br>
Exception Level (see above)<br>
not Register Width: 0=64, 1=32<br>
Stack pointer select: 0=SP0, 1=SPx<span style="color: #008000;"><em> [AArch32 only]</em></span><br>
Cumulative saturation flag<em><span style="color: #008000;"> [AArch32 only]</span></em><br>
Greater than or Equal flags<em><span style="color: #008000;"> [AArch32 only]</span></em><br>
If-then execution state bits <span style="color: #008000;"><em>[AArch32 only]</em></span><br>
J execution state bit <span style="color: #008000;"><em>[AArch32 only]</em></span><br>
T32 execution state bit<span style="color: #008000;"><em> [AArch632 only]</em></span><br>
Endian execution state bit <span style="color: #008000;"><em>[AArch32 only]</em></span><br>
Mode field (see above)<span style="color: #008000;"><em> [AArch32 only]</em></span></td>
</tr>
</tbody>
</table>

The first four flags are the Condition flags `(NZCV)`, and they are the mostly used by processors:

- N: Negative condition flag. If the result is regarded as a two’s complement signed integer, then the PE sets N to 1 if the result is negative, and sets N to 0 if it is positive or zero.

- Z: Zero condition flag. Set to 1 if the result of the instruction is zero, and to 0 otherwise. A result of zero often indicates an equal result from a comparison.

- C: Carry condition flag. Set to 1 if the instruction results in a carry condition, for example an unsigned overflow that is the result of an addition.

- V: Overflow condition flag. Set to 1 if the instruction results in an overflow condition, for example a signed overflow that is the result of an addition


##Condition code suffixes

This suffixes are used by the Branch conditionally instruction, here a table useful to understand what they mean:


<table summary="Condition code suffixes" border="1">
<colgroup>
<col>
<col>
<col> </colgroup>
<thead>
<tr>
<th>Suffix</th>
<th>Flags</th>
<th>Meaning</th>
</tr>
</thead>
<tbody>
<tr>
<td><code>EQ</code></td>
<td><code>Z</code> set</td>
<td>Equal</td>
</tr>
<tr>
<td><code>NE</code></td>
<td><code>Z</code> clear</td>
<td>Not equal</td>
</tr>
<tr>
<td><code>CS or HS</code></td>
<td><code>C</code> set</td>
<td>Higher or same (unsigned &gt;= )</td>
</tr>
<tr>
<td><code>CC or LO</code></td>
<td><code>C</code> clear</td>
<td>Lower (unsigned &lt; )</td>
</tr>
<tr>
<td><code>MI</code></td>
<td><code>N</code> set</td>
<td>Negative</td>
</tr>
<tr>
<td><code>PL</code></td>
<td><code>N</code> clear</td>
<td>Positive or zero</td>
</tr>
<tr>
<td><code>VS</code></td>
<td><code>V</code> set</td>
<td>Overflow</td>
</tr>
<tr>
<td><code>VC</code></td>
<td><code>V</code> clear</td>
<td>No overflow</td>
</tr>
<tr>
<td><code>HI</code></td>
<td><code>C</code> set and <code>Z</code> clear</td>
<td>Higher (unsigned &gt;)</td>
</tr>
<tr>
<td><code>LS</code></td>
<td><code>C</code> clear or <code>Z</code> set</td>
<td>Lower or same (unsigned &lt;=)</td>
</tr>
<tr>
<td><code>GE</code></td>
<td><code>N</code> and <code>V</code> the same</td>
<td>Signed &gt;=</td>
</tr>
<tr>
<td><code>LT</code></td>
<td><code>N</code> and <code>V</code> differ</td>
<td>Signed &lt;</td>
</tr>
<tr>
<td><code>GT</code></td>
<td><code>Z</code> clear, <code>N</code> and <code>V</code> the same</td>
<td>Signed &gt;</td>
</tr>
<tr>
<td><code>LE</code></td>
<td><code>Z</code> set, <code>N</code> and <code>V</code> differ</td>
<td>Signed &lt;=</td>
</tr>
<tr>
<td><code>AL</code></td>
<td>Any</td>
<td>Always. This suffix is normally omitted.</td>
</tr>
</tbody>
</table>

when you see` <cond>` near an assembly instruction you can use one of these suffixes.

##Istruction Set

The A64 encoding structure breaks down into the following functional
groups:

- A miscellaneous group of branch instructions, exception generating instructions, and system instructions.
- Data processing instructions associated with general-purpose registers. These instructions are supported by two functional groups, depending on whether the operands:
    - Are all held in registers.
    - Include an operand with a constant immediate value.
- Load and store instructions associated with the general-purpose register file and the SIMD and floating-point register file.
- SIMD and scalar floating-point data processing instructions that operate on the SIMD and floating-point registers. (I don’t debate)

##What instructions are not present compared to AArch32:

- Conditional execution operations, cause of:
    - The A64 instruction set does not include the concept of predicated or conditional execution. Benchmarking shows that modern branch predictors work well enough that predicated execution of instructions does not offer sufficient benefit to justify its significant use of opcode space, and its implementation cost in advanced implementations. [source]

- Load Multiple.  instructions load from memory a subset, or possibly all, of the general-purpose registers and the PC, so there aren’t: push, pop, ldmia, ecc… : these are be replace by load/store pair.
- Coprocessor instructions


##Branches & Exception

Conditional branch
Conditional branches change the flow of execution depending on the current state of the condition flags or the value in a general-purpose register.


<table border="1">
<tbody>
<tr>
<td><strong>B&lt;cond&gt;</strong></td>
<td>Branch conditionally</td>
<td>B.&lt;cond&gt; &lt;label&gt;</td>
</tr>
<tr>
<td><strong>CBNZ</strong></td>
<td>Compare and branch if nonzero</td>
<td>CBNZ &lt;Wt|Xt&gt;, &lt;label&gt;</td>
</tr>
<tr>
<td><strong>CBZ</strong></td>
<td>Compare and branch if zero</td>
<td>CBZ &lt;Xt&gt;, &lt;label&gt;</td>
</tr>
</tbody>
</table>

##Unconditional branch

<table border="1">
<tbody>
<tr>
<td><strong>B</strong></td>
<td>Branch unconditionally</td>
<td>B &lt;label&gt;</td>
</tr>
<tr>
<td><strong>BL</strong></td>
<td>Branch with link</td>
<td>BL &lt;label&gt;</td>
</tr>
</tbody>
</table>

The BL instruction(s) writes the address of the sequentially following instruction, for the return (see RET), to general-purpose register, X30.

## Unconditional branch (register)

<table border="1">
<tbody>
<tr>
<td><strong>BLR</strong></td>
<td>Branch with link to register</td>
<td>BLR &lt;Xn&gt;</td>
</tr>
<tr>
<td><strong>BR</strong></td>
<td>Branch to register</td>
<td>BR &lt;Xn&gt;</td>
</tr>
<tr>
<td><strong>RET</strong></td>
<td>Return from subroutine:</td>
<td>RET {&lt;Xn&gt;}<span style="color: #008000;">;&nbsp; where Xn register holding the address to be branched to. Defaults to X30 if absent.</span></td>
</tr>
</tbody>
</table>


## Exception generating


- HVC Generate exception targeting Exception level 2
- SMC Generate exception targeting Exception level 3
- SVC Instruction Generate exception targeting Exception level 1

## Others instrunctions

- NOP: No OPeration
- WFE Wait for event
- WFI Wait for interrupt
- SEV Send event
- SEVL Send event local

## Load/Store register

There’re many instructions in this class to move many data size: byte, halfword and word, but I show only four, just to make you understand them : two for move single register and two for move a pair of registers; but first I have to describe how we can access to memory.


## Load/Store addressing modes

This part is very important to understand different ARM addressing modes; the most used are three:

- [base{, #imm}]: Base plus offset  addressing means that the address is the value in the 64-bit base register plus an offset.
    - Example: ldrsw    x0, [x29,76]   #load signed word in x0
- [base, #imm]! : Pre-indexed addressing means that the address is the sum of the value in the 64-bit base register and an offset, and the address is then writtenback to the base register.
    - Example: stp    x29, x30, [sp, -80]!  #store x9 e x30 into stack from sp-80
- [base], #imm : Post-indexed addressing means that the address is the value in the 64-bit base register, and the sum of the  address and the offset is then written back to the base register.
    - Example: ldp    x29, x30, [sp], 80 #load values from stack

now I can describe load/store instructions, don’t care addressing mode, I show you only few example.

##Single Register
Save a register into a memory

UIC Academy

Search UIC...
Community
Downloads
Library
About Us
You are in: UIC > Development > Introduction to ARMv8 64-bit Architecture
Introduction to ARMv8 64-bit Architecture
april 9, 2014 by pnuic
Introduction

The ARM architecture is a Reduced Instruction Set Computer (RISC) architecture, indeed its originally stood for “Acorn RISC Machine” but now stood for “Advanced RISC Machines”.
In the last years, ARM processors, with the diffusion of smartphones and tablets, are beginning very popular: mostly this is due to reduced costs, and a more power efficiency compared to other architectures as CISC:

Complex Instruction Set Computer (CISC) processors, like the x86, have a rich instruction set capable of doing complex things with a single instruction. Such processors often have significant amounts of internal logic that decode machine instructions to sequences of internal operations (microcode).RISC architectures, in contrast, have a smaller number of more general purpose instructions, that might be executed with significantly fewer transistors, making the silicon cheaper and more power efficient. Like other RISC architectures, ARM cores have a large number of general-purpose registers and many instructions execute in a single cycle. It has simple addressing modes, where all load/store addresses can be determined from register contents and instruction fields.

RISC architectures (ARM, Mips, …) peculiarity:

The load/store architecture only allows memory to be accessed by load and store operations, and all values for an operation need to be loaded from memory and be present in registers, so operations as “add reg,[address]” are not permitted!
Another difference with CISC architectures: when a Branch and Link is called (in Intel arch. is the “call” operation) the return address is stored in a special register and not in the stack.
A lack into ARM architecture is the absence of multi-threading support, which is present in many others architectures as: Intel and Mips.
Cause of AArch32 (32bit) is most documented: Arm on wiki, Cambridge University – Operation System Development I decided to talk only about AArch64 (64bit).

Processors:

A short ARM processors list:

Classic or Cortext-A: with DSP, Floating Point, TrustZone e Jazelle extensions. ARMv5 e ARM6 (2001)
Cortex-M: ARM Thumb®-2 technology which provides excellent code density. With Thumb-2 technology, the Cortex-M processors support a fundamental base of 16-bit Thumb instructions, extended to include more powerful 32-bit instructions. First Multi-core. (2004)
Cortex-R: ARMv7 Deeply pipelined micro-architecture,Flexible Multi-Processor Core (MPCore) configurations:Symmetric Multi-Processing (SMP) & Asymmetric Multi-Processing (AMP), LPAE extension.
Cortex-A50: ARMv8-A 64bit with load-acquire and store-release features , which are an excellent match for the C++11, C11 and Java memory models. (2011)
Extensions

With every new version of ARM, there’re new extensions provided, the v8 architecture has these:

Jazelle is a Java hardware/software accelerator: “ARM Jazelle DBX (Direct Bytecode eXecution) technology for direct bytecode execution of Java”. On Sofware side: Jazelle MobileVM is a complete JVM which is Multi-tasking, engineered to provide high performance multi-tasking in a very small memory footprint
Floating Point: for floating point operations
NEON: the ARM SIMD 128 bit (Single instruction, multiple data) engine and DSP the SIMD 32bit engine useful to make linear algebra operations
Cryptographic Extension is an extension of the SIMD support and operates on the vector register file. It provides instructions for the acceleration of encryption and decryption to support the following: AES, SHA1, SHA2-256.
TrustZone: is a system-wide approach to security for a wide array of client and server computing platforms include payment protection technology, digital rights management, BYOD, and a host of secured enterprise solutions
Virtualization Extensions with the Large Physical Address Extension (LPAE) enable the efficient implementation of virtual machine hypervisors for ARM architecture compliant processors.
The visualization extensions provide the basis for ARM architecture compliant processors to address the needs of both client and server devices for the partitioning and management of complex software environments into virtual machines.
The Large Physical Address extension provides the means for each of the software environments to utilize efficiently the available physical memory when handling large amounts of data
V5_to_V8_Architecture

Architectures

AArch64 the ARMv8-A 64-bit execution state, that uses 31 64-bit general purpose registers (R0-R30), and a 64-bit program counter (PC), stack pointer (SP), and exception link registers(ELR). Provides 32 128-bit registers for SIMD vector and scalar floating-point support (V0-V31).
A64 instructions have a fixed length of 32 bits and are always little-endian.
AArch32 is the ARMv8-A 32-bit execution state, that uses 13 32-bit general purpose registers (R0-R12), a 32-bit program counter (PC), stack pointer (SP), and link register (LR). Provides 32 64-bit registers for Advanced SIMD vector and scalar floating-point support.
AArch32 execution state provides a choice of two instruction sets, A32 (ARM) and T32 (Thumb2). Operation in AArch32 state is compatible with ARMv7-A operation.
T32: 16-bit instructions are decompressed transparently to full 32-bit ARM instructions in real time without performance loss.Thumb-2 technology made Thumb a mixed (32- and 16-bit) length instruction set
Data types

Data types are simply these:

Byte: 8 bits.
Halfword: 16 bits.
Word: 32 bits.
Doubleword: 64 bits.
Quadword: 128 bits.
The architecture also supports the following floating-point data types:

Half-precision floating-point formats.
Single-precision floating-point format.
Double-precision floating-point format.
In this short guide, I don’t talk about floating point assembly instructions to don’t make it too long, if you want know more about, you can see the ARM Architecture Reference Manual.

Exception levels

There’re four exception levels, which replaces the 8 different processor modes, they work as the ring in Intel architectures, they are a form of privilege hierarchy:

EL0 is the least privileged level, indeed it is called unprivileged execution. Apps are runned here.
EL1: here can be runned OS kernel
EL2: provides support for virtualization of Non-secure operation. Hypervisor can runned here.
EL3 provides support for switching between two Security states, Secure state and Non-secure state. Secure monitor can be runned here.
When executing in AArch64 state, execution can move between Exception levels only on taking an exception or on returning from an exception.
Each of the 4 privilege levels has 3 private banked registers: the Exception Link Register, Stack Pointer and Saved PSR.

Interprocessing: AArch64 <=> AArch32

Interprocessing is the term used to describe moving between the AArch64 and AArch32 Execution states.
The Execution state can change only on a change of Exception level. This means that the Execution state can change only on taking an exception to a higher Exception level, or returning from an exception to a lower Exception level.
On taking an exception to a higher Exception level, the Execution state either:

Remains unchanged.
Changes from AArch32 state to AArch64 state.
On returning from an exception to a lower Exception level, the Execution state either:

Remains unchanged.
Changes from AArch64 state to AArch32 state.
The A64 Register

A64 has 31 general-purpose registers (integer) more the zero register and the current stack pointer register, here all the registers:

Wn	32 bits	General-purpose register: n can be 0-30
Xn	64 bits	General-purpose register: n can be 0-30
WZR	32 bits	Zero register
XZR	64 bits	Zero register
WSP	32 bits	Current stack pointer
SP	64 bits	Current stack pointer
reg

How registers should be using by compilers and programmers:

r30 (LR): The Link Register, is used as the subroutine link register (LR) and stores the return address when Branch with Link operations are performed.
r29 (FP): The Frame Pointer
r19…r28: Callee-saved registers
r18: The Platform Register, if needed; otherwise a temporary register.
r17 (IP1): The second intra-procedure-call temporary register (can be used by call veneers and PLT code); at other times may be used as a temporary register.
r16 (IP0): The first intra-procedure-call scratch register (can be used by call veneers and PLT code); at other times may be used as a temporary register.
r9…r15: Temporary registers
r8: Indirect result location register
r0…r7: Parameter/result registers
The PC (program counter) has a limited access, only few instructions, as BL and ADL, can modify it.

The use of Stack

The stack implementation is full-descending: in a push the stack pointer is decremented, i.e the stack grows towards lower address.
Another features is that stack must be quad-word aligned: SP mod 16 = 0.

A64 instructions can use the stack pointer only in a limited number of cases:

Load/Store instructions use the current stack pointer as the base address: When stack alignment checking is enabled by system software and the base register is SP, the current stack pointer must be initially quadword aligned, That is, it must be aligned to 16 bytes. Misalignment generates a Stack Alignment fault.
Add and subtract data processing instructions in their immediate and extended register forms, use the current stack pointer as a source register or the destination register or both.
Logical data processing instructions in their immediate form use the current stack pointer as the destination register.
Process State

PSTATE (process state, CPSR on AArch32) holds process state related information, his flags will be change with compare instructions, for example, so it is used by processor to see if make a branch (jump in Intel terminology) or not.

N,
Z,
C,
V,
D,
A,
I,
F,
SS,
IL,
EL,
nRW,
SP,
Q,
GE,
IT,
J,
T,
E,
M	Negative condition flag
Zero condition flag
Carry condition flag
oVerflow condition flag
Debug mask bit [AArch64 only]
Asynchronous abort mask bit
IRQ mask bit
FIQ mask bit
Software step bit
Illegal execution state bit
Exception Level (see above)
not Register Width: 0=64, 1=32
Stack pointer select: 0=SP0, 1=SPx [AArch32 only]
Cumulative saturation flag [AArch32 only]
Greater than or Equal flags [AArch32 only]
If-then execution state bits [AArch32 only]
J execution state bit [AArch32 only]
T32 execution state bit [AArch632 only]
Endian execution state bit [AArch32 only]
Mode field (see above) [AArch32 only]
The first four flags are the Condition flags (NZCV), and they are the mostly used by processors:

N: Negative condition flag. If the result is regarded as a two’s complement signed integer, then the PE sets N to 1 if the result is negative, and sets N to 0 if it is positive or zero.
Z: Zero condition flag. Set to 1 if the result of the instruction is zero, and to 0 otherwise. A result of zero often indicates an equal result from a comparison.
C: Carry condition flag. Set to 1 if the instruction results in a carry condition, for example an unsigned overflow that is the result of an addition.
V: Overflow condition flag. Set to 1 if the instruction results in an overflow condition, for example a signed overflow that is the result of an addition
Condition code suffixes

This suffixes are used by the Branch conditionally instruction, here a table useful to understand what they mean:

Suffix	Flags	Meaning
EQ	Z set	Equal
NE	Z clear	Not equal
CS or HS	C set	Higher or same (unsigned >= )
CC or LO	C clear	Lower (unsigned < )
MI	N set	Negative
PL	N clear	Positive or zero
VS	V set	Overflow
VC	V clear	No overflow
HI	C set and Z clear	Higher (unsigned >)
LS	C clear or Z set	Lower or same (unsigned <=)
GE	N and V the same	Signed >=
LT	N and V differ	Signed <
GT	Z clear, N and V the same	Signed >
LE	Z set, N and V differ	Signed <=
AL	Any	Always. This suffix is normally omitted.
when you see <cond> near an assembly instruction you can use one of these suffixes.

Istruction Set

The A64 encoding structure breaks down into the following functional
groups:

A miscellaneous group of branch instructions, exception generating instructions, and system instructions.
Data processing instructions associated with general-purpose registers. These instructions are supported by two functional groups, depending on whether the operands:
Are all held in registers.
Include an operand with a constant immediate value.
Load and store instructions associated with the general-purpose register file and the SIMD and floating-point register file.
SIMD and scalar floating-point data processing instructions that operate on the SIMD and floating-point registers. (I don’t debate)
What instructions are not present compared to AArch32:

Conditional execution operations, cause of:
The A64 instruction set does not include the concept of predicated or conditional execution. Benchmarking shows that modern branch predictors work well enough that predicated execution of instructions does not offer sufficient benefit to justify its significant use of opcode space, and its implementation cost in advanced implementations. [source]

Load Multiple.  instructions load from memory a subset, or possibly all, of the general-purpose registers and the PC, so there aren’t: push, pop, ldmia, ecc… : these are be replace by load/store pair.
Coprocessor instructions
Branches & Exception

Conditional branch
Conditional branches change the flow of execution depending on the current state of the condition flags or the value in a general-purpose register.

B<cond>	Branch conditionally	B.<cond> <label>
CBNZ	Compare and branch if nonzero	CBNZ <Wt|Xt>, <label>
CBZ	Compare and branch if zero	CBZ <Xt>, <label>
 

Unconditional branch

B	Branch unconditionally	B <label>
BL	Branch with link	BL <label>
The BL instruction(s) writes the address of the sequentially following instruction, for the return (see RET), to general-purpose register, X30.

Unconditional branch (register)

BLR	Branch with link to register	BLR <Xn>
BR	Branch to register	BR <Xn>
RET	Return from subroutine:	RET {<Xn>};  where Xn register holding the address to be branched to. Defaults to X30 if absent.
 

Exception generating

HVC Generate exception targeting Exception level 2
SMC Generate exception targeting Exception level 3
SVC Instruction Generate exception targeting Exception level 1
Others instrunctions

NOP: No OPeration
WFE Wait for event
WFI Wait for interrupt
SEV Send event
SEVL Send event local
Load/Store register

There’re many instructions in this class to move many data size: byte, halfword and word, but I show only four, just to make you understand them : two for move single register and two for move a pair of registers; but first I have to describe how we can access to memory.

Load/Store addressing modes

This part is very important to understand different ARM addressing modes; the most used are three:

[base{, #imm}]: Base plus offset  addressing means that the address is the value in the 64-bit base register plus an offset.
Example: ldrsw    x0, [x29,76]   #load signed word in x0
[base, #imm]! : Pre-indexed addressing means that the address is the sum of the value in the 64-bit base register and an offset, and the address is then writtenback to the base register.
Example: stp    x29, x30, [sp, -80]!  #store x9 e x30 into stack from sp-80
[base], #imm : Post-indexed addressing means that the address is the value in the 64-bit base register, and the sum of the  address and the offset is then written back to the base register.
Example: ldp    x29, x30, [sp], 80 #load values from stack
now I can describe load/store instructions, don’t care addressing mode, I show you only few example.

Single Register
Save a register into a memory

- ldr: Load register works with:
    - Register offset:  LDR <Xt>, [<Xn|SP>, <R><m>{, <extend> {<amount>}}]
    - Immediate offset: LDR <Xt>, [<Xn|SP>], #<simm>
    - PC-relative literal: LDR <Xt>, <label
- str: Store register:
    - register offset: STR <Xt>, [<Xn|SP>, <R><m>{, <extend> {<amount>}}]
    - immediate offset: STR <Xt>, [<Xn|SP>], #<simm>

<simm> is signed immediate byte offset, in the range -256 to 255

## Pair of Registers

Save the two registers specified into memory address of Xn or SP
- ldp load pair: LDP <Xt1>, <Xt2>, [<Xn|SP>], #<imm>
- stp store pair: STP <Xt1>, <Xt2>, [<Xn|SP>], #<imm>
<imm> is signed immediate byte offset, a  multiple of 8 in the range -512 to 504

##Data processing – immediate

Arithmetic (immediate)

<table border="1">
<tbody>
<tr>
<td><strong>ADD</strong></td>
<td>ADD (immediate)</td>
<td>ADD &lt;Xd|SP&gt;, &lt;Xn|SP&gt;, #&lt;imm&gt;{, &lt;shift&gt;}<span style="color: #008000;">; Rd = Rn + shift(imm)</span></td>
</tr>
<tr>
<td><strong>ADDS</strong></td>
<td>Add and set flags</td>
<td></td>
</tr>
<tr>
<td><strong>SUB</strong></td>
<td>Subtract</td>
<td>&nbsp;SUB &lt;Xd|SP&gt;, &lt;Xn|SP&gt;, #&lt;imm&gt;{, &lt;shift&gt;}<span style="color: #008000;">;&nbsp;Rd = Rn – shift(imm)</span></td>
</tr>
<tr>
<td><strong>SUBS</strong></td>
<td>Subtract and set flags</td>
<td></td>
</tr>
<tr>
<td><strong>CMP</strong></td>
<td>Compare</td>
<td>&nbsp;CMP &lt;Xn|SP&gt;, #&lt;imm&gt;{, &lt;shift&gt;}</td>
</tr>
<tr>
<td><strong>CMN</strong></td>
<td>Compare negative</td>
<td></td>
</tr>
</tbody>
</table>

Where: <shift> Is the optional shift type to be applied to the second source operand, defaulting to LSL.
The shift operators LSL (logical shift left), ASR (arithm sift right) and LSR (logical shift right) accept an immediate shift <amount> in the range 0 to one less than the register width of the instruction, inclusive.

## Logical
<table border="1">
<tbody>
<tr>
<td><strong>AND</strong></td>
<td>Bitwise</td>
<td>AND &lt;Xd|SP&gt;, &lt;Xn&gt;, #&lt;imm&gt;&nbsp;<span style="color: #008000;"> ;Rd = Rn AND imm</span></td>
</tr>
<tr>
<td><strong>ANDS</strong></td>
<td>Bitwise AND and set flags</td>
<td>ANDS &lt;Xd&gt;, &lt;Xn&gt;, #&lt;imm&gt;<span style="color: #008000;"> ;Rd = Rn AND imm</span></td>
</tr>
<tr>
<td><strong>EOR</strong></td>
<td>Bitwise exclusive</td>
<td>EOR &lt;Xd|SP&gt;, &lt;Xn&gt;, #&lt;imm&gt;<span style="color: #008000;"> ;Rd = Rn EOR imm</span></td>
</tr>
<tr>
<td><strong>ORR</strong></td>
<td>Bitwise inclusive</td>
<td>ORR &lt;Xd|SP&gt;, &lt;Xn&gt;, #&lt;imm&gt;<span style="color: #008000;"> ;Rd = Rn OR imm</span></td>
</tr>
<tr>
<td><strong>TST</strong></td>
<td>Test bits</td>
<td>TST &lt;Xn&gt;, #&lt;imm&gt;&nbsp; <span style="color: #008000;">;Rn AND imm</span></td>
</tr>
</tbody>
</table>

##Move
Instructions to move wide immediate (16bit):

<table border="1">
<tbody>
<tr>
<td><strong>MOVZ</strong></td>
<td>Move wide with zero</td>
<td>&nbsp;MOVZ &lt;Xd&gt;, #&lt;imm&gt;{, LSL #&lt;shift&gt;}<span style="color: #008000;"> ;Rd = LSL (imm16, shift)</span></td>
</tr>
<tr>
<td><strong>MOVN</strong></td>
<td>Move wide with NOT</td>
<td>&nbsp;MOVN &lt;Xd&gt;, #&lt;imm&gt;{, LSL #&lt;shift&gt;}<span style="color: #008000;"> ;Rd = NOT (LSL (imm16, shift))</span></td>
</tr>
<tr>
<td><strong>MOVK</strong></td>
<td>Move 16-bit immediate into register, keeping other bits unchange</td>
<td>&nbsp;MOVK &lt;Xd&gt;, #&lt;imm&gt;{, LSL #&lt;shift&gt;} <span style="color: #008000;">;&nbsp;Rd&lt;shift+15:shift&gt; = imm16</span></td>
</tr>
</tbody>
</table>


There are also an instruction to move immediate:
```c
MOV <Xd>, #<imm>  ;Rd = imm
```
but his three versions are aliases of movz, movn and movk

## PC-relative address calculation

- The ADR instruction adds a signed, 21-bit immediate to the value of the program counter that fetched this instruction, and then writes the result to a general-purpose register:
ADR <Xd>, <label>

- The ADRP instruction permits the calculation of the address at a 4KB aligned memory region. In conjunction with an ADD(immediate) instruction, or  a Load/Store instruction with a 12-bit immediate offset, this allows for the calculation of, or access to, any address within ±4GB of the current PC:
ADRP <Xd>, <label>

## Shift

<table border="1">
<tbody>
<tr>
<td><strong>ASR</strong></td>
<td>Arithmetic shift right</td>
<td>&nbsp;ASR &lt;Xd&gt;, &lt;Xn&gt;, #&lt;bits to shift&gt;</td>
</tr>
<tr>
<td><strong>LSL</strong></td>
<td>Logical shift left</td>
<td>&nbsp;LSL &lt;Xd&gt;, &lt;Xn&gt;, #&lt;shift&gt;</td>
</tr>
<tr>
<td><strong>LSR</strong></td>
<td>Logical shift right</td>
<td>&nbsp;LSR &lt;Xd&gt;, &lt;Xn&gt;, #&lt;shift&gt;</td>
</tr>
<tr>
<td><strong>ROR</strong></td>
<td>Rotate right</td>
<td>&nbsp;ROR &lt;Xd&gt;, &lt;Xs&gt;, #&lt;bits to shift&gt;</td>
</tr>
</tbody>
</table>

##Data processing – register

###Arithmetic (shifted register)

- ADD: Add
- ADDS: Add and set setting the condition flags
- SUB: Subtract
- SUBS: Subtract and set flags
- CMN: Compare negative
- CMP: Compare
- NEG: Negate ;
- Rd = 0 – shift(Rm, amount)
- NEGS: Negate and set flags


How ADD works, the others are similar:
```c
ADD <Xd>, <Xn>, <Xm>{, <shift> #<amount>}
Rd = Rn + shift(Rm, amount);
```
There’re also the `Arithmetic with carry` instructions which accept two source registers, with the carry flag as an additional input to the calculation and don’t support shift.

- ADC: Add with carry

```c
 ADC <Xd>, <Xn>, <Xm>
```

- ADCS: Add with carry and set flags

```c
ADCS <Xd>, <Xn>, <Xm> ;Rd = Rn + Rm + C
```

- SBC: Subtract with carry

```c
SBC <Xd>, <Xn>, <Xm> ;Rd = Rn – Rm – 1 + C
```

- SBCS: Subtract with carry and set flags
- NGC: Negate with carry

```c
NGC <Xd>, <Xm>  ;Rd = 0 – Rm – 1 + C
```

- NGCS: Negate with carry and set flags


## Logical (shifted register)

- AND: Bitwise AND

```c
ANDS: Bitwise AND and set flags
```

- BIC: Bitwise bit clear

```c
Rd = Rn AND NOT shift(Rm, amount)
```

- BICS: Bitwise bit clear and set flags
- EON: Bitwise exclusive OR NOT

```c
Rd = Rn EOR NOT shift(Rm, amount)
```

- EOR: Bitwise exclusive OR

```c
Rd = Rn EOR shift(Rm, amount)
```

- ORR: Bitwise inclusive OR
- MVN: Bitwise NOT

```c
Rd = NOT shift(Rm, amount)
```

- ORN: Bitwise inclusive OR NOT

```c
Rd = Rn OR NOT shift(Rm, amount)
```

- TST: Test bits

```c
Rn AND shift(Rm, amount)
```

##How they work:
```c
AND <Xd>, <Xn>, <Xm>{, <shift> #<amount>}
Rd = Rn AND shift(Rm, amount)
```

Here <shift> has the default shift operators more the ROR (rotate right)


## Multiply and divide


- MADD Multiply-add

```c
MADD <Xd>, <Xn>, <Xm>, <Xa>; Rd = Ra + Rn * Rm
```

- MSUB Multiply-subtract
- MNEG Multiply-negate
- MUL Multiply

```c
MUL <Xd>, <Xn>, <Xm>; Rd = Rn * Rm
```

- SMADDL Signed multiply-add long
- SMSUBL Signed multiply-subtract long
- SMNEGL Signed multiply-negate long
- SMULL Signed multiply long
- SMULH Signed multiply high
- UMADDL Unsigned multiply-add long
- UMSUBL Unsigned multiply-subtract long
- UMNEGL Unsigned multiply-negate long
- UMULL Unsigned multiply long
- UMULH Unsigned multiply high
- SDIV Signed divide

```c
SDIV <Xd>, <Xn>, <Xm>; Rd = Rn / Rm
```

- UDIV Unsigned divide


##Move

The Move (register) instructions are aliases for other data processing instructions. They copy a value from a general-purpose register to another general-purpose register or the current stack pointer, or from the current stack pointer to a general-purpose register.
```c
MOV <Xd>, <Xm>
Xd = Xm;
```

##Shift (register)

- ASRV: Arithmetic shift right variable
- LSLV: Logical shift left variable
- LSRV: Logical shift right variable
- RORV: Rotate right variable

###An example:

```
ASRV <Xd>, <Xn>, <Xm>
Rd = ASR(Rn, Rm)
```

There’re `alias` instructions that haven’t the ending V.

##CRC32
The optional CRC32 instructions operate on the general-purpose register file to update a 32-bit CRC value from an input value comprising 1, 2, 4, or 8 bytes.
There are two different classes of CRC instructions, CRC32 and CRC32C, that support two commonly used 32-bit polynomials, known as CRC-32 and CRC-32C.

##Conditional select
The Conditional select instructions select between the first or second source register, depending on the current state of the condition flag

<table border="1">
<tbody>
<tr>
<td><strong>CSEL</strong></td>
<td>Conditional select</td>
<td>CSEL &lt;Xd&gt;, &lt;Xn&gt;, &lt;Xm&gt;, &lt;cond&gt; <span style="color: #008000;">;Rd = if cond then Rn else Rm</span></td>
</tr>
<tr>
<td><strong>CSINC</strong></td>
<td>Conditional select increment</td>
<td>CSINC &lt;Xd&gt;, &lt;Xn&gt;, &lt;Xm&gt;, &lt;cond&gt;<span style="color: #008000;"> ;Rd = if cond then Rn else (Rm</span><br>
<span style="color: #008000;"> + 1)</span></td>
</tr>
<tr>
<td><strong>CSINV</strong></td>
<td>Conditional select inversion</td>
<td>CSINV &lt;Xd&gt;, &lt;Xn&gt;, &lt;Xm&gt;, &lt;cond&gt;&nbsp; <span style="color: #008000;">;Rd = if cond then Rn else NOT (Rm)</span></td>
</tr>
<tr>
<td><strong>CSNEG</strong></td>
<td>Conditional select negation</td>
<td>&nbsp;CSNEG &lt;Xd&gt;, &lt;Xn&gt;, &lt;Xm&gt;, &lt;cond&gt;&nbsp;<span style="color: #008000;"> ;Rd = if cond then Rn else -Rm</span></td>
</tr>
<tr>
<td><strong>CSET</strong></td>
<td>Conditional set</td>
<td>CSET &lt;Xd&gt;, &lt;cond&gt;&nbsp;<span style="color: #008000;"> ;Rd = if cond then 1 else 0</span></td>
</tr>
<tr>
<td><strong>CSETM</strong></td>
<td>Conditional set mask</td>
<td>&nbsp;CSETM &lt;Xd&gt;, &lt;cond&gt;<span style="color: #008000;"> ;Rd = if cond then -1 else 0</span></td>
</tr>
<tr>
<td><strong>CINC</strong></td>
<td>Conditional increment</td>
<td>&nbsp;CINC &lt;Xd&gt;, &lt;Xn&gt;, &lt;cond&gt;<span style="color: #008000;"> ;Rd = if cond then Rn+1 else Rn</span></td>
</tr>
<tr>
<td><strong>CINV</strong></td>
<td>Conditional invert</td>
<td>&nbsp;CINV &lt;Xd&gt;, &lt;Xn&gt;, &lt;cond&gt;<span style="color: #008000;"> ;Rd = if cond then NOT(Rn) else Rn</span></td>
</tr>
<tr>
<td><strong>CNEG</strong></td>
<td>Conditional negate</td>
<td>&nbsp;CNEG &lt;Xd&gt;, &lt;Xn&gt;, &lt;cond&gt;&nbsp;<span style="color: #008000;"> ;Rd = if cond then -Rn else Rn</span></td>
</tr>
</tbody>
</table>

##Conditional comparison
The Conditional comparison instructions provide a conditional select for the NZCV condition flags, setting the flags to the result of an arithmetic comparison of its two source register values if the named input condition is true, or to an immediate value if the input condition is false. There are register and immediate forms. The immediate form compares the source register to a small 5-bit unsigned value.


<table border="1">
<tbody>
<tr>
<td><strong>CCMN</strong></td>
<td>Conditional compare negative (register)</td>
<td>CCMN &lt;Xn&gt;, &lt;Xm&gt;, #&lt;nzcv&gt;, &lt;cond&gt;<span style="color: #008000;">&nbsp; ;flags = if cond then compare(Rn, -Rm) else #nzcv</span></td>
</tr>
<tr>
<td><strong>CCMN</strong></td>
<td>Conditional compare negative (immediate)</td>
<td>CCMN &lt;Xn&gt;, #&lt;imm&gt;, #&lt;nzcv&gt;, &lt;cond&gt;<span style="color: #008000;"> ;flags = if cond then compare(Rn, #-imm) else #nzcv</span></td>
</tr>
<tr>
<td><strong>CCMP</strong></td>
<td>Conditional compare (register)</td>
<td>CCMP &lt;Xn&gt;, &lt;Xm&gt;, #&lt;nzcv&gt;, &lt;cond&gt;<span style="color: #008000;"> ;flags = if cond then compare(Rn, Rm) else #nzcv</span></td>
</tr>
<tr>
<td><strong>CCMP</strong></td>
<td>Conditional compare (immediate)</td>
<td>CCMP &lt;Xn&gt;, #&lt;imm&gt;, #&lt;nzcv&gt;, &lt;cond&gt; <span style="color: #008000;">;flags = if cond then compare(Rn, #imm) else #nzcv</span></td>
</tr>
</tbody>
</table>

- <nzcv> is the flag bit specifier, an immediate in the range 0 to 15, giving the alternative state for the 4-bit NZCV condition flags, encoded in the nzcv field.

- <imm> Is a five bit unsigned (positive) immediate encoded in the imm5 field.


## How ccmop works:
it checks NZCV flags for  <cond>,  if previous comparison passed, do this one and set NZCV, otherwise set NZCV to <imm>.
If we have to write this code:

```c
x0 >= x1 && x2 == x3
```

in arm assembly, with ccmp we can do this:

```c
cmp x0, x1
ccmp x2, x3, #0, ge
beq good
```

##Assembly Example:

It’s time to code!! Like others tutorial on assembly  I show first the C-like code and then ARM asm.


```c
#include "stdio.h"

static int v[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
void print(int i);
int add(int v, int t);

int main()
{

    int i;
    int array[10];

    for (i = 0; i < 10; i++) {
        array[i] = v[i] * (add(i, 5));
    }

    return 0;
}

int add(int v, int t)
{
    return  v + t;
}
```

UIC Academy

Search UIC...
Community
Downloads
Library
About Us
You are in: UIC > Development > Introduction to ARMv8 64-bit Architecture
Introduction to ARMv8 64-bit Architecture
april 9, 2014 by pnuic
Introduction

The ARM architecture is a Reduced Instruction Set Computer (RISC) architecture, indeed its originally stood for “Acorn RISC Machine” but now stood for “Advanced RISC Machines”.
In the last years, ARM processors, with the diffusion of smartphones and tablets, are beginning very popular: mostly this is due to reduced costs, and a more power efficiency compared to other architectures as CISC:

Complex Instruction Set Computer (CISC) processors, like the x86, have a rich instruction set capable of doing complex things with a single instruction. Such processors often have significant amounts of internal logic that decode machine instructions to sequences of internal operations (microcode).RISC architectures, in contrast, have a smaller number of more general purpose instructions, that might be executed with significantly fewer transistors, making the silicon cheaper and more power efficient. Like other RISC architectures, ARM cores have a large number of general-purpose registers and many instructions execute in a single cycle. It has simple addressing modes, where all load/store addresses can be determined from register contents and instruction fields.

RISC architectures (ARM, Mips, …) peculiarity:

The load/store architecture only allows memory to be accessed by load and store operations, and all values for an operation need to be loaded from memory and be present in registers, so operations as “add reg,[address]” are not permitted!
Another difference with CISC architectures: when a Branch and Link is called (in Intel arch. is the “call” operation) the return address is stored in a special register and not in the stack.
A lack into ARM architecture is the absence of multi-threading support, which is present in many others architectures as: Intel and Mips.
Cause of AArch32 (32bit) is most documented: Arm on wiki, Cambridge University – Operation System Development I decided to talk only about AArch64 (64bit).

Processors:

A short ARM processors list:

Classic or Cortext-A: with DSP, Floating Point, TrustZone e Jazelle extensions. ARMv5 e ARM6 (2001)
Cortex-M: ARM Thumb®-2 technology which provides excellent code density. With Thumb-2 technology, the Cortex-M processors support a fundamental base of 16-bit Thumb instructions, extended to include more powerful 32-bit instructions. First Multi-core. (2004)
Cortex-R: ARMv7 Deeply pipelined micro-architecture,Flexible Multi-Processor Core (MPCore) configurations:Symmetric Multi-Processing (SMP) & Asymmetric Multi-Processing (AMP), LPAE extension.
Cortex-A50: ARMv8-A 64bit with load-acquire and store-release features , which are an excellent match for the C++11, C11 and Java memory models. (2011)
Extensions

With every new version of ARM, there’re new extensions provided, the v8 architecture has these:

Jazelle is a Java hardware/software accelerator: “ARM Jazelle DBX (Direct Bytecode eXecution) technology for direct bytecode execution of Java”. On Sofware side: Jazelle MobileVM is a complete JVM which is Multi-tasking, engineered to provide high performance multi-tasking in a very small memory footprint
Floating Point: for floating point operations
NEON: the ARM SIMD 128 bit (Single instruction, multiple data) engine and DSP the SIMD 32bit engine useful to make linear algebra operations
Cryptographic Extension is an extension of the SIMD support and operates on the vector register file. It provides instructions for the acceleration of encryption and decryption to support the following: AES, SHA1, SHA2-256.
TrustZone: is a system-wide approach to security for a wide array of client and server computing platforms include payment protection technology, digital rights management, BYOD, and a host of secured enterprise solutions
Virtualization Extensions with the Large Physical Address Extension (LPAE) enable the efficient implementation of virtual machine hypervisors for ARM architecture compliant processors.
The visualization extensions provide the basis for ARM architecture compliant processors to address the needs of both client and server devices for the partitioning and management of complex software environments into virtual machines.
The Large Physical Address extension provides the means for each of the software environments to utilize efficiently the available physical memory when handling large amounts of data
V5_to_V8_Architecture

Architectures

AArch64 the ARMv8-A 64-bit execution state, that uses 31 64-bit general purpose registers (R0-R30), and a 64-bit program counter (PC), stack pointer (SP), and exception link registers(ELR). Provides 32 128-bit registers for SIMD vector and scalar floating-point support (V0-V31).
A64 instructions have a fixed length of 32 bits and are always little-endian.
AArch32 is the ARMv8-A 32-bit execution state, that uses 13 32-bit general purpose registers (R0-R12), a 32-bit program counter (PC), stack pointer (SP), and link register (LR). Provides 32 64-bit registers for Advanced SIMD vector and scalar floating-point support.
AArch32 execution state provides a choice of two instruction sets, A32 (ARM) and T32 (Thumb2). Operation in AArch32 state is compatible with ARMv7-A operation.
T32: 16-bit instructions are decompressed transparently to full 32-bit ARM instructions in real time without performance loss.Thumb-2 technology made Thumb a mixed (32- and 16-bit) length instruction set
Data types

Data types are simply these:

Byte: 8 bits.
Halfword: 16 bits.
Word: 32 bits.
Doubleword: 64 bits.
Quadword: 128 bits.
The architecture also supports the following floating-point data types:

Half-precision floating-point formats.
Single-precision floating-point format.
Double-precision floating-point format.
In this short guide, I don’t talk about floating point assembly instructions to don’t make it too long, if you want know more about, you can see the ARM Architecture Reference Manual.

Exception levels

There’re four exception levels, which replaces the 8 different processor modes, they work as the ring in Intel architectures, they are a form of privilege hierarchy:

EL0 is the least privileged level, indeed it is called unprivileged execution. Apps are runned here.
EL1: here can be runned OS kernel
EL2: provides support for virtualization of Non-secure operation. Hypervisor can runned here.
EL3 provides support for switching between two Security states, Secure state and Non-secure state. Secure monitor can be runned here.
When executing in AArch64 state, execution can move between Exception levels only on taking an exception or on returning from an exception.
Each of the 4 privilege levels has 3 private banked registers: the Exception Link Register, Stack Pointer and Saved PSR.

Interprocessing: AArch64 <=> AArch32

Interprocessing is the term used to describe moving between the AArch64 and AArch32 Execution states.
The Execution state can change only on a change of Exception level. This means that the Execution state can change only on taking an exception to a higher Exception level, or returning from an exception to a lower Exception level.
On taking an exception to a higher Exception level, the Execution state either:

Remains unchanged.
Changes from AArch32 state to AArch64 state.
On returning from an exception to a lower Exception level, the Execution state either:

Remains unchanged.
Changes from AArch64 state to AArch32 state.
The A64 Register

A64 has 31 general-purpose registers (integer) more the zero register and the current stack pointer register, here all the registers:

Wn	32 bits	General-purpose register: n can be 0-30
Xn	64 bits	General-purpose register: n can be 0-30
WZR	32 bits	Zero register
XZR	64 bits	Zero register
WSP	32 bits	Current stack pointer
SP	64 bits	Current stack pointer
reg

How registers should be using by compilers and programmers:

r30 (LR): The Link Register, is used as the subroutine link register (LR) and stores the return address when Branch with Link operations are performed.
r29 (FP): The Frame Pointer
r19…r28: Callee-saved registers
r18: The Platform Register, if needed; otherwise a temporary register.
r17 (IP1): The second intra-procedure-call temporary register (can be used by call veneers and PLT code); at other times may be used as a temporary register.
r16 (IP0): The first intra-procedure-call scratch register (can be used by call veneers and PLT code); at other times may be used as a temporary register.
r9…r15: Temporary registers
r8: Indirect result location register
r0…r7: Parameter/result registers
The PC (program counter) has a limited access, only few instructions, as BL and ADL, can modify it.

The use of Stack

The stack implementation is full-descending: in a push the stack pointer is decremented, i.e the stack grows towards lower address.
Another features is that stack must be quad-word aligned: SP mod 16 = 0.

A64 instructions can use the stack pointer only in a limited number of cases:

Load/Store instructions use the current stack pointer as the base address: When stack alignment checking is enabled by system software and the base register is SP, the current stack pointer must be initially quadword aligned, That is, it must be aligned to 16 bytes. Misalignment generates a Stack Alignment fault.
Add and subtract data processing instructions in their immediate and extended register forms, use the current stack pointer as a source register or the destination register or both.
Logical data processing instructions in their immediate form use the current stack pointer as the destination register.
Process State

PSTATE (process state, CPSR on AArch32) holds process state related information, his flags will be change with compare instructions, for example, so it is used by processor to see if make a branch (jump in Intel terminology) or not.

N,
Z,
C,
V,
D,
A,
I,
F,
SS,
IL,
EL,
nRW,
SP,
Q,
GE,
IT,
J,
T,
E,
M	Negative condition flag
Zero condition flag
Carry condition flag
oVerflow condition flag
Debug mask bit [AArch64 only]
Asynchronous abort mask bit
IRQ mask bit
FIQ mask bit
Software step bit
Illegal execution state bit
Exception Level (see above)
not Register Width: 0=64, 1=32
Stack pointer select: 0=SP0, 1=SPx [AArch32 only]
Cumulative saturation flag [AArch32 only]
Greater than or Equal flags [AArch32 only]
If-then execution state bits [AArch32 only]
J execution state bit [AArch32 only]
T32 execution state bit [AArch632 only]
Endian execution state bit [AArch32 only]
Mode field (see above) [AArch32 only]
The first four flags are the Condition flags (NZCV), and they are the mostly used by processors:

N: Negative condition flag. If the result is regarded as a two’s complement signed integer, then the PE sets N to 1 if the result is negative, and sets N to 0 if it is positive or zero.
Z: Zero condition flag. Set to 1 if the result of the instruction is zero, and to 0 otherwise. A result of zero often indicates an equal result from a comparison.
C: Carry condition flag. Set to 1 if the instruction results in a carry condition, for example an unsigned overflow that is the result of an addition.
V: Overflow condition flag. Set to 1 if the instruction results in an overflow condition, for example a signed overflow that is the result of an addition
Condition code suffixes

This suffixes are used by the Branch conditionally instruction, here a table useful to understand what they mean:

Suffix	Flags	Meaning
EQ	Z set	Equal
NE	Z clear	Not equal
CS or HS	C set	Higher or same (unsigned >= )
CC or LO	C clear	Lower (unsigned < )
MI	N set	Negative
PL	N clear	Positive or zero
VS	V set	Overflow
VC	V clear	No overflow
HI	C set and Z clear	Higher (unsigned >)
LS	C clear or Z set	Lower or same (unsigned <=)
GE	N and V the same	Signed >=
LT	N and V differ	Signed <
GT	Z clear, N and V the same	Signed >
LE	Z set, N and V differ	Signed <=
AL	Any	Always. This suffix is normally omitted.
when you see <cond> near an assembly instruction you can use one of these suffixes.

Istruction Set

The A64 encoding structure breaks down into the following functional
groups:

A miscellaneous group of branch instructions, exception generating instructions, and system instructions.
Data processing instructions associated with general-purpose registers. These instructions are supported by two functional groups, depending on whether the operands:
Are all held in registers.
Include an operand with a constant immediate value.
Load and store instructions associated with the general-purpose register file and the SIMD and floating-point register file.
SIMD and scalar floating-point data processing instructions that operate on the SIMD and floating-point registers. (I don’t debate)
What instructions are not present compared to AArch32:

Conditional execution operations, cause of:
The A64 instruction set does not include the concept of predicated or conditional execution. Benchmarking shows that modern branch predictors work well enough that predicated execution of instructions does not offer sufficient benefit to justify its significant use of opcode space, and its implementation cost in advanced implementations. [source]

Load Multiple.  instructions load from memory a subset, or possibly all, of the general-purpose registers and the PC, so there aren’t: push, pop, ldmia, ecc… : these are be replace by load/store pair.
Coprocessor instructions
Branches & Exception

Conditional branch
Conditional branches change the flow of execution depending on the current state of the condition flags or the value in a general-purpose register.

B<cond>	Branch conditionally	B.<cond> <label>
CBNZ	Compare and branch if nonzero	CBNZ <Wt|Xt>, <label>
CBZ	Compare and branch if zero	CBZ <Xt>, <label>
 

Unconditional branch

B	Branch unconditionally	B <label>
BL	Branch with link	BL <label>
The BL instruction(s) writes the address of the sequentially following instruction, for the return (see RET), to general-purpose register, X30.

Unconditional branch (register)

BLR	Branch with link to register	BLR <Xn>
BR	Branch to register	BR <Xn>
RET	Return from subroutine:	RET {<Xn>};  where Xn register holding the address to be branched to. Defaults to X30 if absent.
 

Exception generating

HVC Generate exception targeting Exception level 2
SMC Generate exception targeting Exception level 3
SVC Instruction Generate exception targeting Exception level 1
Others instrunctions

NOP: No OPeration
WFE Wait for event
WFI Wait for interrupt
SEV Send event
SEVL Send event local
Load/Store register

There’re many instructions in this class to move many data size: byte, halfword and word, but I show only four, just to make you understand them : two for move single register and two for move a pair of registers; but first I have to describe how we can access to memory.

Load/Store addressing modes

This part is very important to understand different ARM addressing modes; the most used are three:

[base{, #imm}]: Base plus offset  addressing means that the address is the value in the 64-bit base register plus an offset.
Example: ldrsw    x0, [x29,76]   #load signed word in x0
[base, #imm]! : Pre-indexed addressing means that the address is the sum of the value in the 64-bit base register and an offset, and the address is then writtenback to the base register.
Example: stp    x29, x30, [sp, -80]!  #store x9 e x30 into stack from sp-80
[base], #imm : Post-indexed addressing means that the address is the value in the 64-bit base register, and the sum of the  address and the offset is then written back to the base register.
Example: ldp    x29, x30, [sp], 80 #load values from stack
now I can describe load/store instructions, don’t care addressing mode, I show you only few example.

Single Register
Save a register into a memory

ldr: Load register works with:
Register offset:  LDR <Xt>, [<Xn|SP>, <R><m>{, <extend> {<amount>}}]
Immediate offset: LDR <Xt>, [<Xn|SP>], #<simm>
PC-relative literal: LDR <Xt>, <label
str: Store register:
register offset: STR <Xt>, [<Xn|SP>, <R><m>{, <extend> {<amount>}}]
immediate offset: STR <Xt>, [<Xn|SP>], #<simm>
<simm> is signed immediate byte offset, in the range -256 to 255

Pair of Registers
Save the two registers specified into memory address of Xn or SP

ldp load pair: LDP <Xt1>, <Xt2>, [<Xn|SP>], #<imm>
stp store pair: STP <Xt1>, <Xt2>, [<Xn|SP>], #<imm>
<imm> is signed immediate byte offset, a  multiple of 8 in the range -512 to 504

Data processing – immediate

Arithmetic (immediate)

ADD	ADD (immediate)	ADD <Xd|SP>, <Xn|SP>, #<imm>{, <shift>}; Rd = Rn + shift(imm)
ADDS	Add and set flags	
SUB	Subtract	 SUB <Xd|SP>, <Xn|SP>, #<imm>{, <shift>}; Rd = Rn – shift(imm)
SUBS	Subtract and set flags	
CMP	Compare	 CMP <Xn|SP>, #<imm>{, <shift>}
CMN	Compare negative	
Where: <shift> Is the optional shift type to be applied to the second source operand, defaulting to LSL.
The shift operators LSL (logical shift left), ASR (arithm sift right) and LSR (logical shift right) accept an immediate shift <amount> in the range 0 to one less than the register width of the instruction, inclusive.

Logical

AND	Bitwise	AND <Xd|SP>, <Xn>, #<imm>  ;Rd = Rn AND imm
ANDS	Bitwise AND and set flags	ANDS <Xd>, <Xn>, #<imm> ;Rd = Rn AND imm
EOR	Bitwise exclusive	EOR <Xd|SP>, <Xn>, #<imm> ;Rd = Rn EOR imm
ORR	Bitwise inclusive	ORR <Xd|SP>, <Xn>, #<imm> ;Rd = Rn OR imm
TST	Test bits	TST <Xn>, #<imm>  ;Rn AND imm
 

Move
Instructions to move wide immediate (16bit):

MOVZ	Move wide with zero	 MOVZ <Xd>, #<imm>{, LSL #<shift>} ;Rd = LSL (imm16, shift)
MOVN	Move wide with NOT	 MOVN <Xd>, #<imm>{, LSL #<shift>} ;Rd = NOT (LSL (imm16, shift))
MOVK	Move 16-bit immediate into register, keeping other bits unchange	 MOVK <Xd>, #<imm>{, LSL #<shift>} ; Rd<shift+15:shift> = imm16
There are also an instruction to move immediate:
MOV <Xd>, #<imm>  ;Rd = imm
but his three versions are aliases of movz, movn and movk

PC-relative address calculation

The ADR instruction adds a signed, 21-bit immediate to the value of the program counter that fetched this instruction, and then writes the result to a general-purpose register:
ADR <Xd>, <label>
The ADRP instruction permits the calculation of the address at a 4KB aligned memory region. In conjunction with an ADD(immediate) instruction, or  a Load/Store instruction with a 12-bit immediate offset, this allows for the calculation of, or access to, any address within ±4GB of the current PC:
ADRP <Xd>, <label>
Shift

ASR	Arithmetic shift right	 ASR <Xd>, <Xn>, #<bits to shift>
LSL	Logical shift left	 LSL <Xd>, <Xn>, #<shift>
LSR	Logical shift right	 LSR <Xd>, <Xn>, #<shift>
ROR	Rotate right	 ROR <Xd>, <Xs>, #<bits to shift>
Data processing – register

Arithmetic (shifted register)

ADD: Add
ADDS: Add and set setting the condition flags
SUB: Subtract
SUBS: Subtract and set flags
CMN: Compare negative
CMP: Compare
NEG: Negate ;
Rd = 0 – shift(Rm, amount)
NEGS: Negate and set flags
How ADD works, the others are similar:
ADD <Xd>, <Xn>, <Xm>{, <shift> #<amount>}
Rd = Rn + shift(Rm, amount);

There’re also the Arithmetic with carry instructions which accept two source registers, with the carry flag as an additional input to the calculation and don’t support shift.

ADC: Add with carry
ADC <Xd>, <Xn>, <Xm>
ADCS: Add with carry and set flags
ADCS <Xd>, <Xn>, <Xm> ;Rd = Rn + Rm + C
SBC: Subtract with carry
SBC <Xd>, <Xn>, <Xm> ;Rd = Rn – Rm – 1 + C
SBCS: Subtract with carry and set flags
NGC: Negate with carry
NGC <Xd>, <Xm>  ;Rd = 0 – Rm – 1 + C
NGCS: Negate with carry and set flags
Logical (shifted register)

AND: Bitwise AND
ANDS: Bitwise AND and set flags
BIC: Bitwise bit clear
Rd = Rn AND NOT shift(Rm, amount)
BICS: Bitwise bit clear and set flags
EON: Bitwise exclusive OR NOT
Rd = Rn EOR NOT shift(Rm, amount)
EOR: Bitwise exclusive OR
Rd = Rn EOR shift(Rm, amount)
ORR: Bitwise inclusive OR
MVN: Bitwise NOT
Rd = NOT shift(Rm, amount)
ORN: Bitwise inclusive OR NOT
Rd = Rn OR NOT shift(Rm, amount)
TST: Test bits
Rn AND shift(Rm, amount)
How they work:
AND <Xd>, <Xn>, <Xm>{, <shift> #<amount>}
Rd = Rn AND shift(Rm, amount)
Here <shift> has the default shift operators more the ROR (rotate right)

Multiply and divide

MADD Multiply-add
MADD <Xd>, <Xn>, <Xm>, <Xa>; Rd = Ra + Rn * Rm
MSUB Multiply-subtract
MNEG Multiply-negate
MUL Multiply
MUL <Xd>, <Xn>, <Xm>; Rd = Rn * Rm
SMADDL Signed multiply-add long
SMSUBL Signed multiply-subtract long
SMNEGL Signed multiply-negate long
SMULL Signed multiply long
SMULH Signed multiply high
UMADDL Unsigned multiply-add long
UMSUBL Unsigned multiply-subtract long
UMNEGL Unsigned multiply-negate long
UMULL Unsigned multiply long
UMULH Unsigned multiply high
SDIV Signed divide
SDIV <Xd>, <Xn>, <Xm>; Rd = Rn / Rm
UDIV Unsigned divide
Move

The Move (register) instructions are aliases for other data processing instructions. They copy a value from a general-purpose register to another general-purpose register or the current stack pointer, or from the current stack pointer to a general-purpose register.
MOV <Xd>, <Xm>
Xd = Xm;

Shift (register)

ASRV: Arithmetic shift right variable
LSLV: Logical shift left variable
LSRV: Logical shift right variable
RORV: Rotate right variable
An example:
ASRV <Xd>, <Xn>, <Xm>
Rd = ASR(Rn, Rm)
There’re alias instructions that haven’t the ending V.

CRC32
The optional CRC32 instructions operate on the general-purpose register file to update a 32-bit CRC value from an input value comprising 1, 2, 4, or 8 bytes.
There are two different classes of CRC instructions, CRC32 and CRC32C, that support two commonly used 32-bit polynomials, known as CRC-32 and CRC-32C.

Conditional select
The Conditional select instructions select between the first or second source register, depending on the current state of the condition flag

CSEL	Conditional select	CSEL <Xd>, <Xn>, <Xm>, <cond> ;Rd = if cond then Rn else Rm
CSINC	Conditional select increment	CSINC <Xd>, <Xn>, <Xm>, <cond> ;Rd = if cond then Rn else (Rm
+ 1)
CSINV	Conditional select inversion	CSINV <Xd>, <Xn>, <Xm>, <cond>  ;Rd = if cond then Rn else NOT (Rm)
CSNEG	Conditional select negation	 CSNEG <Xd>, <Xn>, <Xm>, <cond>  ;Rd = if cond then Rn else -Rm
CSET	Conditional set	CSET <Xd>, <cond>  ;Rd = if cond then 1 else 0
CSETM	Conditional set mask	 CSETM <Xd>, <cond> ;Rd = if cond then -1 else 0
CINC	Conditional increment	 CINC <Xd>, <Xn>, <cond> ;Rd = if cond then Rn+1 else Rn
CINV	Conditional invert	 CINV <Xd>, <Xn>, <cond> ;Rd = if cond then NOT(Rn) else Rn
CNEG	Conditional negate	 CNEG <Xd>, <Xn>, <cond>  ;Rd = if cond then -Rn else Rn
 

Conditional comparison
The Conditional comparison instructions provide a conditional select for the NZCV condition flags, setting the flags to the result of an arithmetic comparison of its two source register values if the named input condition is true, or to an immediate value if the input condition is false. There are register and immediate forms. The immediate form compares the source register to a small 5-bit unsigned value.

CCMN	Conditional compare negative (register)	CCMN <Xn>, <Xm>, #<nzcv>, <cond>  ;flags = if cond then compare(Rn, -Rm) else #nzcv
CCMN	Conditional compare negative (immediate)	CCMN <Xn>, #<imm>, #<nzcv>, <cond> ;flags = if cond then compare(Rn, #-imm) else #nzcv
CCMP	Conditional compare (register)	CCMP <Xn>, <Xm>, #<nzcv>, <cond> ;flags = if cond then compare(Rn, Rm) else #nzcv
CCMP	Conditional compare (immediate)	CCMP <Xn>, #<imm>, #<nzcv>, <cond> ;flags = if cond then compare(Rn, #imm) else #nzcv
Where:

<nzcv> is the flag bit specifier, an immediate in the range 0 to 15, giving the alternative state for the 4-bit NZCV condition flags, encoded in the nzcv field.
<imm> Is a five bit unsigned (positive) immediate encoded in the imm5 field.
How ccmop works:
it checks NZCV flags for  <cond>,  if previous comparison passed, do this one and set NZCV, otherwise set NZCV to <imm>.
If we have to write this code:
x0 >= x1 && x2 == x3
in arm assembly, with ccmp we can do this:

cmp x0, x1
ccmp x2, x3, #0, ge
beq good
Assembly Example:

It’s time to code!! Like others tutorial on assembly  I show first the C-like code and then ARM asm.

#include "stdio.h"

static int v[] = {1,2,3,4,5,6,7,8,9,10};
void print(int i);
int add(int v, int t);

int main() {

int i;
int array[10];

for(i=0; i < 10; i++)
	array[i] = v[i] * (add(i,5));

return 0;

}

int add(int v, int t) {
 return  v + t;
}
Now this is the asm code generated by GCC, you need to download Linaro GCC to code on ARMv8:


```sh
.cpu generic+fp+simd
	.data
	.align	3
	.type	v, %object
	.size	v, 40
;v array
v:
	.word	1
	.word	2
	.word	3
	.word	4
	.word	5
	.word	6
	.word	7
	.word	8
	.word	9
	.word	10

;dump:
0000000000410918 :
  410918:    00000001     .word    0x00000001
  41091c:    00000002     .word    0x00000002
  410920:    00000003     .word    0x00000003
  410924:    00000004     .word    0x00000004
  410928:    00000005     .word    0x00000005
  41092c:    00000006     .word    0x00000006
  410930:    00000007     .word    0x00000007
  410934:    00000008     .word    0x00000008
  410938:    00000009     .word    0x00000009
  41093c:    0000000a     .word    0x0000000a
; end dump

	.text
	.align	2
	.global	main
	.type	main, %function
main:
	stp	x29, x30, [sp, -80]! ;save register into sp-80 and sp-88, and free memory for array
;remember the Pre-indexed addressing
	add	x29, sp, 0 ; frame pointer = stack pointer
	str	x19, [sp,16] ;store r19 - remember Base plus offset

;first loop
	str	wzr, [x29,76] ;i=0 -> wzr: zero register
	b	.L2 ;branch to label
.L3:
	adrp	x0, v ;calc label address  --> dump:   adrp    x0, 410000 
	add	x1, x0, :lo12:v   ; --> dump:    add    x1, x0, #0x918 see above 0x410918 dump
	ldrsw	x0, [x29,76] ;load signed word (i variable)
	lsl	x0, x0, 2 ;logical shift left (as mult for 2^2), it need to calc i-offset
	add	x0, x1, x0
	ldr	w19, [x0] ; w19 = v[i]
	ldr	w0, [x29,76] ;remember [x29,76] is i
;remeber w0 is paramer register
	mov	w1, 5 ;w1 is a param register
	bl	add  ;call add(w0, w1)
	mul	w1, w19, w0 ;w0 after a bl has result value
;w1 = v[i] * add(w0,w1)
	add	x2, x29, 32 ;array base address: FP+32
	ldrsw	x0, [x29,76] ;load i variable
	lsl	x0, x0, 2 ;calc the
	add	x0, x2, x0  ;array[i] offset as for v[i]
	str	w1, [x0] ;save w1 into x0 address

	ldr	w0, [x29,76]
	add	w0, w0, 1 ; i += 1
	str	w0, [x29,76]
.L2:
	ldr	w0, [x29,76]
	cmp	w0, 9
	ble	.L3 ; if i <= 9 re-start loop
;end of first for cicle
	mov	w0, 0  ;w0 is the result register in this case
	ldr	x19, [sp,16]  ;re-load old x19 value
	ldp	x29, x30, [sp], 80 ;re-load old frame pointer and return address
	.size	main, .-main
	.section	.rodata

	.align	2
	.global	add
	.type	add, %function

add:
;start of generic prologue
	sub	sp, sp, #16 ;free memory for 2 register
	str	w0, [sp,12] ; save the first param
	str	w1, [sp,8] ;save the second param
;end of prologue
;code
	ldr	w1, [sp,12] ;load the first param
	ldr	w0, [sp,8] ;load second param
	add	w0, w1, w0 ;w0 has the result value

;epilogue
	add	sp, sp, 16 ;free the stack
	ret   ;return to address  in x30
	.size	add, .-add
```

To run this code, you can use ARM Foundation Model (it’s free) how you see here: the Hello World in ARMv8

##Reference:

- Arm Architetture
- 2012 LLVM Developers’ Meeting: The AArch64 backend: status and plans
- ARM Goes 64-bit
