# ntohs, ntohl, htons,htonl的比较


最近的工作是进行程序由 SUN SPARC 向 Intel X86 移植的工作，多数问题都出现在网络字节序的部分，所以拿出来在这部分工作中使用频率较高的几个接口区分比较下：）

ntohs =net to host short int 16位

htons=host to net short int 16位

ntohl =net to host long int 32位

htonl=host to net   long int   32位

##ntohs 简述：

将一个无符号短整形数从网络字节顺序转换为主机字节顺序。

```c
    #include 
    u_short PASCAL FAR ntohs( u_short netshort);
    netshort：一个以网络字节顺序表达的16位数。
```

注释：
    本函数将一个16位数由网络字节顺序转换为主机字节顺序。
返回值：ntohs()返回一个以主机字节顺序表达的数。

##htons 简述：

将主机的无符号短整形数转换成网络字节顺序。 
```c
    #include 
    u_short PASCAL FAR htons( u_short hostshort); 
    hostshort：主机字节顺序表达的16位数。 
```

注释： 
    本函数将一个16位数从主机字节顺序转换成网络字节顺序。 
返回值： htons()返回一个网络字节顺序的值。

---

这2个函数提供了主机字节顺序与网络字节顺序的转换

比如网络字节 为 00 01

u_short    a;如何直接对应的话    a=0100; 为什么呢？因为主机是从高字节到低字节的，所以应该转化后

a=ntohs(0001); 这样 a=0001;

首先，假设你已经有了一个sockaddr_in结构体ina，你有一个IP地址”132.241.5.10″ 要储存在其中，你就要用到函数inet_addr(),将IP地址从 点数格式转换成无符号长整型。使用方法如下：
```c
ina.sin_addr.s_addr = inet_addr(“132.241.5.10″);
```

注意，inet_addr()返回的地址已经是网络字节格式，所以你无需再调用 函数htonl()。
我们现在发现上面的代码片断不是十分完整的，因为它没有错误检查。 显而易见，当inet_addr()发生错误时返回-1。记住这些二进制数字？(无符 号数)-1仅和IP地址255.255.255.255相符合！这可是广播地址！大错特 错！记住要先进行错误检查。
好了，现在你可以将IP地址转换成长整型了。有没有其相反的方法呢？ 它可以将一个in_addr结构体输出成点数格式？这样的话，你就要用到函数 inet_ntoa()(“ntoa”的含义是”network to ascii”)，就像这样： 

printf(“%s”,inet_ntoa(ina.sin_addr));

它将输出IP地址。需要注意的是inet_ntoa()将结构体in-addr作为一 个参数，不是长整形。同样需要注意的是它返回的是一个指向一个字符的 指针。它是一个由inet_ntoa()控制的静态的固定的指针，所以每次调用 inet_ntoa()，它就将覆盖上次调用时所得的IP地址。例如：

```c
char *a1, *a2;
a1 = inet_ntoa(ina1.sin_addr); /* 这是198.92.129.1 */
a2 = inet_ntoa(ina2.sin_addr); /* 这是132.241.5.10 */
printf(“address 1: %s “,a1);
printf(“address 2: %s “,a2);
```
输出如下：

```sh
address 1: 132.241.5.10
address 2: 132.241.5.10
```

假如你需要保存这个IP地址，使用strcopy()函数来指向你自己的字符指针。

==================================================

htonl()表示将32位的主机字节顺序转化为32位的网络字节顺序 htons()表示将16位的主机字节顺序转化为16位的网络字节顺序（ip地址是32位的端口号是16位的 ）

```
inet_ntoa()
```

简述：
    将网络地址转换成“.”点隔的字符串格式。

```c
    #include

    char FAR* PASCAL FAR inet_ntoa( struct in_addr in);

    in：一个表示Internet主机地址的结构。
```

注释：
    本函数将一个用in参数所表示的Internet地址结构转换成以“.” 间隔的诸如“a.b.c.d”的字符串形式。请注意inet_ntoa()返回的字符串存放在WINDOWS套接口实现所分配的内存中。应用程序不应假设 该内存是如何分配的。在同一个线程的下一个WINDOWS套接口调用前，数据将保证是有效。

返回值：
    若无错误发生，inet_ntoa()返回一个字符指针。否则的话，返回NVLL。其中的数据应在下一个WINDOWS套接口调用前复制出来。

参见：
    inet_addr().
    
    
    
```c
测试代码如下
include 
#include 
#include 
#include 
#include 
int main(int aargc, char* argv[])
{
         struct in_addr addr1,addr2;
         ulong   l1,l2;
         l1= inet_addr(“192.168.0.74″);
         l2 = inet_addr(“211.100.21.179″);
         memcpy(&addr1, &l1, 4);
         memcpy(&addr2, &l2, 4);
         printf(“%s : %s “, inet_ntoa(addr1), inet_ntoa(addr2));    //注意这一句的运行结果
         printf(“%s “, inet_ntoa(addr1));
         printf(“%s “, inet_ntoa(addr2));
         return 0;
}
实际运行结果如下：
192.168.0.74 : 192.168.0.74       //从这里可以看出,printf里的inet_ntoa只运行了一次。
192.168.0.74
211.100.21.179

inet_ntoa返回一个char *,而这个char *的空间是在inet_ntoa里面静态分配的，所以inet_ntoa后面的调用会覆盖上一次的调用。第一句printf的结果只能说明在printf里面的可变参数的求值是从右到左的，仅此而已。
```
