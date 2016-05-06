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
    

