## The Art of Writing (C)ode in the UNIX Environment

The purpose of this project is to create a repository with a lot of source
code well documented, it is totally focused on the beautiful art of writing 
UNIX programs in C, c11 mainly.

### How to compile programs

`$gcc   -std=c11 -Wall -pedantic source.c`
`$clang -std=c11 -Wall -pedantic source.c`

Sometimes we need a macro like _BSD_SOURCE or _GNU_SOURCE to compile.

A list of compilers used right now:

* PowerPC Architecture (32 bit, big-endian)
  * gcc (Debian 4.9.2-10) 4.9.2
  * Debian clang version 3.5.0-10 (based on LLVM 3.5.0)

* x86_32 and x86_64 (32, 64 bit, little endian)
  * gcc (Debian 5.3.1-8) 5.3.1
  * Debian clang version 3.7.1-1 (based on LLVM 3.7.1)

### Bibliography

- **Unix Programming**
  * **The Unix Programming Environment**. *Brian W.Kernighan*, *Rob Pike* (`1983, Prentice-Hall`);
  * **Advanced Unix Programming**. *Marc J.Rochkind* (`2nd, 2004, Addison Wesley`);
  * **The Linux Programming Interface: A Linux and UNIX System Programming Handbook**. *Michael Kerrisk* (`2010, No Starch Press`);
  * **Advanced Programming in the UNIX Environment**. *Richard Stevens*, *Stephen A.Rago* (`3rd, 2013, Addison Wesley`);

<hr />

<address>`Luca Suriano a.k.a. Behemoth`: `<behemoth _at_ autistici _dot_ org>`
`GPG KeyID`: `0xE83F9FBB`</address>
