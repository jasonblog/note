# gcc編譯，ld連接

```c
#include <stdio.h>

int main(void)
{
    printf("Hello world\n");
    return 0;
}
```

```sh
gcc -v hello.c -o hello
```


```sh
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/5/lto-wrapper
Target: x86_64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 5.4.0-6ubuntu1~16.04.2' --with-bugurl=file:///usr/share/doc/gcc-5/README.Bugs --enable-languages=c,ada,c++,java,go,d,fortran,objc,obj-c++ --prefix=/usr --program-suffix=-5 --enable-shared --enable-linker-build-id --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --libdir=/usr/lib --enable-nls --with-sysroot=/ --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --with-default-libstdcxx-abi=new --enable-gnu-unique-object --disable-vtable-verify --enable-libmpx --enable-plugin --with-system-zlib --disable-browser-plugin --enable-java-awt=gtk --enable-gtk-cairo --with-java-home=/usr/lib/jvm/java-1.5.0-gcj-5-amd64/jre --enable-java-home --with-jvm-root-dir=/usr/lib/jvm/java-1.5.0-gcj-5-amd64 --with-jvm-jar-dir=/usr/lib/jvm-exports/java-1.5.0-gcj-5-amd64 --with-arch-directory=amd64 --with-ecj-jar=/usr/share/java/eclipse-ecj.jar --enable-objc-gc --enable-multiarch --disable-werror --with-arch-32=i686 --with-abi=m64 --with-multilib-list=m32,m64,mx32 --enable-multilib --with-tune=generic --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu
Thread model: posix
gcc version 5.4.0 20160609 (Ubuntu 5.4.0-6ubuntu1~16.04.2) 
COLLECT_GCC_OPTIONS='-Wall' '-g' '-Wpedantic' '-o' 'hello' '-v' '-mtune=generic' '-march=x86-64'
 /usr/lib/gcc/x86_64-linux-gnu/5/cc1 -quiet -v -imultiarch x86_64-linux-gnu hello.c -quiet -dumpbase hello.c -mtune=generic -march=x86-64 -auxbase hello -g -Wall -Wpedantic -version -fstack-protector-strong -Wformat-security -o /tmp/ccWBz7Fj.s
GNU C11 (Ubuntu 5.4.0-6ubuntu1~16.04.2) version 5.4.0 20160609 (x86_64-linux-gnu)
	compiled by GNU C version 5.4.0 20160609, GMP version 6.1.0, MPFR version 3.1.4, MPC version 1.0.3
GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
ignoring nonexistent directory "/usr/local/include/x86_64-linux-gnu"
ignoring nonexistent directory "/usr/lib/gcc/x86_64-linux-gnu/5/../../../../x86_64-linux-gnu/include"
#include "..." search starts here:
#include <...> search starts here:
 /usr/lib/gcc/x86_64-linux-gnu/5/include
 /usr/local/include
 /usr/lib/gcc/x86_64-linux-gnu/5/include-fixed
 /usr/include/x86_64-linux-gnu
 /usr/include
End of search list.
GNU C11 (Ubuntu 5.4.0-6ubuntu1~16.04.2) version 5.4.0 20160609 (x86_64-linux-gnu)
	compiled by GNU C version 5.4.0 20160609, GMP version 6.1.0, MPFR version 3.1.4, MPC version 1.0.3
GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
Compiler executable checksum: eb080614e8c0415bfb1aad7fb88ffefc
COLLECT_GCC_OPTIONS='-Wall' '-g' '-Wpedantic' '-o' 'hello' '-v' '-mtune=generic' '-march=x86-64'
 as -v --64 -o /tmp/ccaY85ef.o /tmp/ccWBz7Fj.s
GNU assembler version 2.26.1 (x86_64-linux-gnu) using BFD version (GNU Binutils for Ubuntu) 2.26.1
COMPILER_PATH=/usr/lib/gcc/x86_64-linux-gnu/5/:/usr/lib/gcc/x86_64-linux-gnu/5/:/usr/lib/gcc/x86_64-linux-gnu/:/usr/lib/gcc/x86_64-linux-gnu/5/:/usr/lib/gcc/x86_64-linux-gnu/
LIBRARY_PATH=/usr/lib/gcc/x86_64-linux-gnu/5/:/usr/lib/gcc/x86_64-linux-gnu/5/../../../x86_64-linux-gnu/:/usr/lib/gcc/x86_64-linux-gnu/5/../../../../lib/:/lib/x86_64-linux-gnu/:/lib/../lib/:/usr/lib/x86_64-linux-gnu/:/usr/lib/../lib/:./:/usr/lib/gcc/x86_64-linux-gnu/5/../../../:/lib/:/usr/lib/
COLLECT_GCC_OPTIONS='-Wall' '-g' '-Wpedantic' '-o' 'hello' '-v' '-mtune=generic' '-march=x86-64'
 /usr/lib/gcc/x86_64-linux-gnu/5/collect2 -plugin /usr/lib/gcc/x86_64-linux-gnu/5/liblto_plugin.so -plugin-opt=/usr/lib/gcc/x86_64-linux-gnu/5/lto-wrapper -plugin-opt=-fresolution=/tmp/cceiXwPa.res -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lgcc_s -plugin-opt=-pass-through=-lc -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lgcc_s --sysroot=/ --build-id --eh-frame-hdr -m elf_x86_64 --hash-style=gnu --as-needed -dynamic-linker /lib64/ld-linux-x86-64.so.2 -z relro -o hello /usr/lib/gcc/x86_64-linux-gnu/5/../../../x86_64-linux-gnu/crt1.o /usr/lib/gcc/x86_64-linux-gnu/5/../../../x86_64-linux-gnu/crti.o /usr/lib/gcc/x86_64-linux-gnu/5/crtbegin.o -L/usr/lib/gcc/x86_64-linux-gnu/5 -L/usr/lib/gcc/x86_64-linux-gnu/5/../../../x86_64-linux-gnu -L/usr/lib/gcc/x86_64-linux-gnu/5/../../../../lib -L/lib/x86_64-linux-gnu -L/lib/../lib -L/usr/lib/x86_64-linux-gnu -L/usr/lib/../lib -L. -L/usr/lib/gcc/x86_64-linux-gnu/5/../../.. /tmp/ccaY85ef.o -lgcc --as-needed -lgcc_s --no-as-needed -lc -lgcc --as-needed -lgcc_s --no-as-needed /usr/lib/gcc/x86_64-linux-gnu/5/crtend.o /usr/lib/gcc/x86_64-linux-gnu/5/../../../x86_64-linux-gnu/crtn.o
```


### gcc -c 

```sh
gcc -c -o hello.o hello.c
```

### ld
```sh
ld -o hello -dynamic-linker /lib64/ld-linux-x86-64.so.2 \
-z relro -o hello /usr/lib/gcc/x86_64-linux-gnu/5/../../../x86_64-linux-gnu/crt1.o \
/usr/lib/gcc/x86_64-linux-gnu/5/../../../x86_64-linux-gnu/crti.o \
/usr/lib/gcc/x86_64-linux-gnu/5/crtbegin.o \
-L/usr/lib/gcc/x86_64-linux-gnu/5 \
-L/usr/lib/gcc/x86_64-linux-gnu/5/../../../x86_64-linux-gnu \
-L/usr/lib/gcc/x86_64-linux-gnu/5/../../../../lib \
-L/lib/x86_64-linux-gnu -L/lib/../lib \-L/usr/lib/x86_64-linux-gnu \
-L/usr/lib/../lib \
-L. \
-L/usr/lib/gcc/x86_64-linux-gnu/5/../../.. \
-lgcc --as-needed -lgcc_s \
--no-as-needed -lc -lgcc --as-needed -lgcc_s \
--no-as-needed /usr/lib/gcc/x86_64-linux-gnu/5/crtend.o \
/usr/lib/gcc/x86_64-linux-gnu/5/../../../x86_64-linux-gnu/crtn.o hello.o
```
### run

```
./hello
```