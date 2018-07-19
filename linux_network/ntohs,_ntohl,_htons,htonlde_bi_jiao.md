# ntohs, ntohl, htons,htonl的比較


最近的工作是進行程序由 SUN SPARC 向 Intel X86 移植的工作，多數問題都出現在網絡字節序的部分，所以拿出來在這部分工作中使用頻率較高的幾個接口區分比較下：）

ntohs =net to host short int 16位

htons=host to net short int 16位

ntohl =net to host long int 32位

htonl=host to net   long int   32位

##ntohs 簡述：

將一個無符號短整形數從網絡字節順序轉換為主機字節順序。

```c
    #include 
    u_short PASCAL FAR ntohs( u_short netshort);
    netshort：一個以網絡字節順序表達的16位數。
```

註釋：
    本函數將一個16位數由網絡字節順序轉換為主機字節順序。
返回值：ntohs()返回一個以主機字節順序表達的數。

##htons 簡述：

將主機的無符號短整形數轉換成網絡字節順序。 
```c
    #include 
    u_short PASCAL FAR htons( u_short hostshort); 
    hostshort：主機字節順序表達的16位數。 
```

註釋： 
    本函數將一個16位數從主機字節順序轉換成網絡字節順序。 
返回值： htons()返回一個網絡字節順序的值。

---

這2個函數提供了主機字節順序與網絡字節順序的轉換

比如網絡字節 為 00 01

u_short    a;如何直接對應的話    a=0100; 為什麼呢？因為主機是從高字節到低字節的，所以應該轉化後

a=ntohs(0001); 這樣 a=0001;

首先，假設你已經有了一個sockaddr_in結構體ina，你有一個IP地址”132.241.5.10″ 要儲存在其中，你就要用到函數inet_addr(),將IP地址從 點數格式轉換成無符號長整型。使用方法如下：
```c
ina.sin_addr.s_addr = inet_addr(“132.241.5.10″);
```

注意，inet_addr()返回的地址已經是網絡字節格式，所以你無需再調用 函數htonl()。
我們現在發現上面的代碼片斷不是十分完整的，因為它沒有錯誤檢查。 顯而易見，當inet_addr()發生錯誤時返回-1。記住這些二進制數字？(無符 號數)-1僅和IP地址255.255.255.255相符合！這可是廣播地址！大錯特 錯！記住要先進行錯誤檢查。
好了，現在你可以將IP地址轉換成長整型了。有沒有其相反的方法呢？ 它可以將一個in_addr結構體輸出成點數格式？這樣的話，你就要用到函數 inet_ntoa()(“ntoa”的含義是”network to ascii”)，就像這樣： 

printf(“%s”,inet_ntoa(ina.sin_addr));

它將輸出IP地址。需要注意的是inet_ntoa()將結構體in-addr作為一 個參數，不是長整形。同樣需要注意的是它返回的是一個指向一個字符的 指針。它是一個由inet_ntoa()控制的靜態的固定的指針，所以每次調用 inet_ntoa()，它就將覆蓋上次調用時所得的IP地址。例如：

```c
char *a1, *a2;
a1 = inet_ntoa(ina1.sin_addr); /* 這是198.92.129.1 */
a2 = inet_ntoa(ina2.sin_addr); /* 這是132.241.5.10 */
printf(“address 1: %s “,a1);
printf(“address 2: %s “,a2);
```
輸出如下：

```sh
address 1: 132.241.5.10
address 2: 132.241.5.10
```

假如你需要保存這個IP地址，使用strcopy()函數來指向你自己的字符指針。

==================================================

htonl()表示將32位的主機字節順序轉化為32位的網絡字節順序 htons()表示將16位的主機字節順序轉化為16位的網絡字節順序（ip地址是32位的端口號是16位的 ）

```
inet_ntoa()
```

簡述：
    將網絡地址轉換成“.”點隔的字符串格式。

```c
    #include

    char FAR* PASCAL FAR inet_ntoa( struct in_addr in);

    in：一個表示Internet主機地址的結構。
```

註釋：
    本函數將一個用in參數所表示的Internet地址結構轉換成以“.” 間隔的諸如“a.b.c.d”的字符串形式。請注意inet_ntoa()返回的字符串存放在WINDOWS套接口實現所分配的內存中。應用程序不應假設 該內存是如何分配的。在同一個線程的下一個WINDOWS套接口調用前，數據將保證是有效。

返回值：
    若無錯誤發生，inet_ntoa()返回一個字符指針。否則的話，返回NVLL。其中的數據應在下一個WINDOWS套接口調用前複製出來。

參見：
    inet_addr().
    
    
    
```c
測試代碼如下
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
         printf(“%s : %s “, inet_ntoa(addr1), inet_ntoa(addr2));    //注意這一句的運行結果
         printf(“%s “, inet_ntoa(addr1));
         printf(“%s “, inet_ntoa(addr2));
         return 0;
}
實際運行結果如下：
192.168.0.74 : 192.168.0.74       //從這裡可以看出,printf裡的inet_ntoa只運行了一次。
192.168.0.74
211.100.21.179

inet_ntoa返回一個char *,而這個char *的空間是在inet_ntoa裡面靜態分配的，所以inet_ntoa後面的調用會覆蓋上一次的調用。第一句printf的結果只能說明在printf裡面的可變參數的求值是從右到左的，僅此而已。
```
