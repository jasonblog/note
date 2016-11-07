# 如何在linux下編譯彙編程序，內功心法是彙編程序的結構


the process for writing and building Linux assembly-language programs , the structure of assembly-language programs, and a few assembly-language commands.

本章教會你的招式是如何在linux下編譯彙編程序，內功心法是彙編程序的結構。

麻雀雖小，五臟俱全，這個代碼很小，但包含了彙編的基本結構

```asm
# PURPOSE:
#          Simple program that exits and returns a 
#          status code back to the Linux kernel

# INPUT:   none

# OUTPUT:  returns a status code.This can be viewed by typing 'echo $'?

#          after running the program

# VARIABLES:
#          %eax holds the system call number
#          %ebx holds the return status

#
.section .data
.section .text
.globl _start
_start:
    movl $1, %eax         # this is the linux kernel command
                          # number (system call) for exiting
                          # a program

    movl $0, %ebx         # this is the status number we will
                          # return to the operating system.
                          # Change this around and it will
                          # return different things to
                          # echo $?

    int $0x80             # this wakes up the kernel to run
                          # the exit command

```

What you have typed in is called the source code. Source code is the human-readable form of a program. In order to transform it into a program that a computer can run, we need to assemble and link it.

這是源代碼，源代碼是一種人類可以看懂的程序形式。源代碼經過編譯(assembel)和鏈接(link)後就可以轉換成可運行的程序。

The first step is to assemble it. Assembling is the process that transforms what you typed into instructions for the machine. The machine itself only reads sets of numbers, but humans prefer words. An assembly language is a more human-readable form of the instructions a computer understands. Assembling transforms the human-readable file into a machine-readable one. To assembly the program type in the command

```asm
as exit.s -o exit.o
```

第一步(assemble)是將源代碼(human-readable)轉換成機器指令(machine-readable)，機器只能讀懂1010的二進制代碼(set of number)，人讀單詞更方便 ，而彙編是比機器指令更好閱讀的形式。這個過程用as彙編器來執行的命令.

```sh
as exit.s -o exit.o
```

as is the command which runs the assembler,exit.s is the source file, and -o exit.o tells the assemble to put it’s output in the file exit.o. exit.o is an object file. An object file is code that is in the machine’s language, but has not been completely put together.

as是as彙編器,exit.s是源代碼文件，exit.o告訴彙編器將輸出放在exit.o中，linux命令中的寄存器-o制定輸出文件.exit.o是目標文件(object file). 目標文件由機器語言寫成的代碼文件,但在這裡還沒有完整。

In most large programs, you will have several source files, and you will convert each one into an object file. The linker is the program that is responsible for putting the object files together and adding information to it so that the kernel knows how to load and run it. In our case, we only have one object file, so the linker is only adding the information to enable it to run. To link the file, enter the command

```sh
ld exit.o -o exit
```

大部分大程序中會包含很多個源文件，需要把每一個都轉換成目標文件。鏈接器(linker)正是將這些目標文件放在一起的程序，它附加了一些信息使得內核知道如何加載(load)和運行(run)程序。load程序加載是內核的事情，源程序代碼放在硬盤或者其他地方，運行前需要將他加載進內存，這個過程就是load。我們這裡只有一個.obj目標文件，只需要加入使它能運行的信息即可。to link the file,enter ...

ld is the command to run the linker, exit.o is the object file we want to link, and -o exit instructs the linker to output the new program into a file called exit.If any of these commands reported errors, you have either mistyped your program or the command. After correcting the program, you have to re-run all the commands.

You must always re-assemble and re-link programs after you modify the source file for the changes to occur in the program. You can run exit by typing in the command

```sh
./exit
```

The ./ is used to tell the computer that the program isn’t in one of the normal program directories, but is the current directory instead 2 . You will notice when you type this command, the only thing that happens is that you’ll go to the next line.

That’s because this program does nothing but exit. However, immediately after you run the program, if you type in

```sh
echo $?
```

It will say 0 . What is happening is that every program when it exits gives Linux an exit status code, which tells it if everything went all right.If everything was okay, it returns 0. UNIX programs return numbers other than zero to indicate failure or other errors,warnings, or statuses . The programmer determines what each number means. You can view this code by typing in echo $? . In the following section we will look at what each part of the code does.

##Outline of an Assembly Language Program

從例子中分析彙編代碼結構

Take a look at the program we just entered. At the beginning there are lots of lines that begin with hashes ( # ). These are comments. Comments are not translated by the assembler.

以#開頭的是注視，注視不會被編譯器翻譯成機器代碼

They are used only for the programmer to talk to anyone who looks at the code in the future. Most programs you write will be modified by others. Get into the habit of writing comments in your code that will help them understand both why the program exists and how it works. Always include the following in your comments:

大部分你寫的程序會其他人修改，註釋便於理解代碼。註釋最好包含如下：

- The purpose of the code (目的，為了解決什麼問題)
- An overview of the processing involved (框架結構)
- Anything strange your program does and why it does it 3 (bug和解決)

After the comments, the next line says

```sh
.section .data
```

Anything starting with a period isn’t directly translated into a machine instruction. Instead, it’s an instruction to the assembler itself.These are called assembler directives or pseudo-operations because they are handled by the assembler and are not actually run by the computer. The .section command breaks your program up into sections. This command starts the data section, where you list any memory storage you will need for data. Our program doesn’t use any, so we don’t need the section. It’s just here for completeness. Almost every program you write in the future will have data.

以.開頭的叫偽指令或者彙編指示器，偽指令是為了指導彙編器，彙編器會處理這些指令，而不會直接被翻譯成機器代碼。比如，.section將程序分割成多個section...

```sh
.section .text
```

which starts the text section. The text section of a program is where the program instructions live.

```sh
.globl _start
```

This instructs the assembler that _start is important to remember. _start is a symbol, which means that it is going to be replaced by something else either during assembly or linking. Symbols are generally used to mark locations of programs or data, so you can refer to them by name instead of by their location number. Imagine if you had to refer to every memory location by it’s address. First of all, it would be very confusing because you would have to memorize or look up the numeric memory address of every piece of code or data. In addition, every time you had to insert a piece of data or code you would have to change all the addresses in your program! Symbols are used so that the assembler and linker can take care of keeping track of addresses, and you can concentrate on writing your program.

.globl means that the assembler shouldn’t discard this symbol after assembly, because the linker will need it. _start is a special symbol that always needs to be marked with .globl because it marks the location of the start of the program. Without marking this location in this way, when the computer loads your program it won’t know where to begin running your program.