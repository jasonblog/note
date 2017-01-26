# 动态库及静态库的制作步骤讲解


首先回答前面的问题，一共有多少种方法来指定告诉linux共享库链接器ld.so已经编译好的库libbase.so的位置呢？答案是一共有五种，它们都可以通知ld.so去哪些地方找下已经编译好的c语言函数动态库，它们是：

1）ELF可执行文件中动态段中DT_RPATH所指定的路径。即在编译目标代码时, 对gcc加入链接参数“-Wl,-rpath”指定动态库搜索路径，eg：gcc -Wl,-rpath,/home/arc/test,-rpath,/lib/,-rpath,/usr/lib/,-rpath,/usr/local/lib test.c

2）环境变量LD_LIBRARY_PATH 指定的动态库搜索路径

3）/etc/ld.so.cache中所缓存的动态库路径，这个可以通过先修改配置文件/etc/ld.so.conf中指定的动态库搜索路径，然后执行ldconfig命令来改变。

4）默认的动态库搜索路径/lib

5）默认的动态库搜索路径/usr/lib

另外：在嵌入式Linux系统的实际应用中，1和2被经常使用，也有一些相对简单的的嵌入式系统会采用4或5的路径来规范动态库，3在嵌入式系统中使用的比较少, 因为有很多系统根本就不支持ld.so.cache。

那么，动态链接器ld.so在这五种路径中，是按照什么样的顺序来搜索需要的动态共享库呢？答案这里先告知就是按照上面的顺序来得，即优先级是：`1-->2-->3-->4-->5`。我们可以写简单的程序来证明这个结论。

 
```c
pt1.c
void pt()
{
    printf("1  path on the gcc give \n");
}

pt2.c
#include <stdio.h>

void pt()
{

    printf("2 path on the LD_LIBRARY_PATH \n");

}

pt3.c
#include <stdio.h>

void pt()
{
    printf("3 path on the /etc/ld.so.conf \n");
}

pt4.c
#include <stdio.h>
void pt()
{
    printf("4 path on the /lib \n");
}

pt5.c
#include <stdio.h>
void pt()
{
    printf("5 path on the /usr/lib \n");
}
```

然后，分别编译这5个函数，然后将它们分别移到上面5种情况对应的5个不同目录下：


```sh
gcc -fPIC -c pt1.c -o pt.o
gcc -shared pt.o -o libpt.so
mv libpt.so /tmp/st/1/

gcc -fPIC -c pt2.c -o pt.o
gcc -shared pt.o -o libpt.so
mv libpt.so /tmp/st/2/

gcc -fPIC -c pt3.c -o pt.o
gcc -shared pt.o -o libpt.so
mv libpt.so /tmp/st/3/

gcc -fPIC -c pt4.c -o pt.o
gcc -shared pt.o -o libpt.so
mv libpt.so /lib/

gcc -fPIC -c pt5.c -o pt.o
gcc -shared pt.o -o libpt.so
mv libpt.so /usr/lib/
```

再次，编写一个main函数m，让它来调用函数pt：

- m.c

```c
#include <stdio.h>

int main()
{
    printf("start....\n");
    pt();
    printf("......end\n");

    return 0;
}
```

最后，准备环境，让ld都知道这5个路径：

```sh 
(a) 往/etc/ld.so.conf总增加一行，内容：/tmp/st/3，然后执行 ldconfig 命令

(b) export LD_LIBRARY_PATH=/tmp/st/2
```

另外3中路径，ld都可以得到，请接着看下面。

之后测试：

```sh
gcc m.c -o m1 -L/tmp/st/1 -lpt -Wl,-rpath,/tmp/st/1
./m1
```

```sh
start....

1  path on the gcc give

......end
```

这里在可执行文件中动态段中DT_RPATH所指定的路径，因此需要在编译m.c的时候就指定路径，由于其他路径都也告诉了ld，很明显，此种方法优先级最高了。

```sh
gcc m.c -o m -L/tmp/st/1 -lpt
./m
```

```sh
start....

2 path on the LD_LIBRARY_PATH

......end
```

这里很显然调用了LD_LIBRARY_PATH指定了路径中的共享库，因此此种情况优先级第二。

```sh
mv /tmp/st/2/libpt.so /tmp/st/2/libpt2.so
./m
```

```sh
start....

3 path on the /etc/ld.so.conf

......end
```

这里是调用了/etc/ld.so.cache中所缓存的动态库路径中的共享库，因此此种情况优先级第三。

```sh
mv /tmp/st/3/libpt.so /tmp/st/3/libpt3.so
./m
```

```sh
start....

4 path on the /lib

......end
```

这里是调用/lib中的共享库，优先级第四。

```sh
rm /lib/libpt.so
./m
```

```sh
start....

5 path on the /usr/lib

......end
```
这里是调用/lib中的共享库，优先级第五。

故证明这五种路径指定方法的优先级是1-->2-->3-->4-->5！

---

一般我们在Linux下执行某些外部程序的时候可能会提示找不到共享库的错误, 比如:

 

tmux: error while loading shared libraries: libevent-1.4.so.2: cannot open shared object file: No such file or directory


原因一般有两个, 一个是操作系统里确实没有包含该共享库(lib*.so.*文件)或者共享库版本不对, 遇到这种情况那就去网上下载并安装上即可. 

另外一个原因就是已经安装了该共享库, 但执行需要调用该共享库的程序的时候, 程序按照默认共享库路径找不到该共享库文件. 

所以安装共享库后要注意共享库路径设置问题, 如下:

###1) 如果共享库文件安装到了/lib或/usr/lib目录下, 那么需执行一下ldconfig命令

ldconfig命令的用途, 主要是在默认搜寻目录(/lib和/usr/lib)以及动态库配置文件/etc/ld.so.conf内所列的目录下, 搜索出可共享的动态链接库(格式如lib*.so*), 进而创建出动态装入程序(ld.so)所需的连接和缓存文件. 缓存文件默认为/etc/ld.so.cache, 此文件保存已排好序的动态链接库名字列表. 

###2) 如果共享库文件安装到了/usr/local/lib(很多开源的共享库都会安装到该目录下)或其它"非/lib或/usr/lib"目录下, 那么在执行ldconfig命令前, 还要把新共享库目录加入到共享库配置文件/etc/ld.so.conf中, 如下:

```sh
# cat /etc/ld.so.conf
include ld.so.conf.d/*.conf
# echo "/usr/local/lib" >> /etc/ld.so.conf
# ldconfig
```

### 3) 如果共享库文件安装到了其它"非/lib或/usr/lib" 目录下,  但是又不想在/etc/ld.so.conf中加路径(或者是没有权限加路径). 那可以export一个全局变量LD_LIBRARY_PATH, 然后运行程序的时候就会去这个目录中找共享库. 

LD_LIBRARY_PATH的意思是告诉loader在哪些目录中可以找到共享库. 可以设置多个搜索目录, 这些目录之间用冒号分隔开. 比如安装了一个mysql到/usr/local/mysql目录下, 其中有一大堆库文件在/usr/local/mysql/lib下面, 则可以在.bashrc或.bash_profile或shell里加入以下语句即可:

```sh
export LD_LIBRARY_PATH=/usr/local/mysql/lib:$LD_LIBRARY_PATH    
```

一般来讲这只是一种临时的解决方案, 在没有权限或临时需要的时候使用.

--End--

---

假设在math目录下已编辑好add.c sub.c div.c mul.c func_point.c文件，func_point.c为包含main（）的源文件！
动态库的制作：

###方法一：

```sh
gcc -c -fPIC add.c sub.c div.c mul.c //-c表示生成.o目标文件,-f后加一些编译选项，PIC表示与位置无关
gcc -shared -o libmymath.so add.o sub.o mul.o div.o//创建共享库mymath，添加add.o,sub.o,mul.o,div.o目标文件
sudo mv libmymath.so /usr/lib
gcc func_point.c -lmymath//-l后面加动态链接库名字
```

###方法二：

```sh
gcc -c -fPIC add.c sub.c div.c mul.c //-c:生成.o目标文件,-f后加一些编译选项，PIC表示与位置无关
gcc -shared -o libmymath.so add.o sub.o mul.o div.o//创建共享库mymath，添加add.o，sub.o mul.o div.o目标文件
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.//注意此操作后把当前目录加入环境变量中
gcc func_point.c -L. -lmymath//-l后面加动态链接库名字
```

###方法三：
```sh
gcc -c -fPIC add.c sub.c div.c mul.c //-c:生成.o目标文件,-f后加一些编译选项，PIC表示与位置无关
gcc -shared -o libmymath.so add.o sub.o mul.o div.o//创建共享库mymath，添加add.o，sub.o mul.o div.o目标文件
sudo vi /etc/ld.so.conf//把你的动态库路径加入此文件中
sudo ldconfig
gcc func_point.c -L. -lmymath//-l后面加动态链接库名字
```

