# 動態庫及靜態庫的製作步驟講解


首先回答前面的問題，一共有多少種方法來指定告訴linux共享庫鏈接器ld.so已經編譯好的庫libbase.so的位置呢？答案是一共有五種，它們都可以通知ld.so去哪些地方找下已經編譯好的c語言函數動態庫，它們是：

1）ELF可執行文件中動態段中DT_RPATH所指定的路徑。即在編譯目標代碼時, 對gcc加入鏈接參數“-Wl,-rpath”指定動態庫搜索路徑，eg：gcc -Wl,-rpath,/home/arc/test,-rpath,/lib/,-rpath,/usr/lib/,-rpath,/usr/local/lib test.c

2）環境變量LD_LIBRARY_PATH 指定的動態庫搜索路徑

3）/etc/ld.so.cache中所緩存的動態庫路徑，這個可以通過先修改配置文件/etc/ld.so.conf中指定的動態庫搜索路徑，然後執行ldconfig命令來改變。

4）默認的動態庫搜索路徑/lib

5）默認的動態庫搜索路徑/usr/lib

另外：在嵌入式Linux系統的實際應用中，1和2被經常使用，也有一些相對簡單的的嵌入式系統會採用4或5的路徑來規範動態庫，3在嵌入式系統中使用的比較少, 因為有很多系統根本就不支持ld.so.cache。

那麼，動態鏈接器ld.so在這五種路徑中，是按照什麼樣的順序來搜索需要的動態共享庫呢？答案這裡先告知就是按照上面的順序來得，即優先級是：`1-->2-->3-->4-->5`。我們可以寫簡單的程序來證明這個結論。

 
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

然後，分別編譯這5個函數，然後將它們分別移到上面5種情況對應的5個不同目錄下：


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

再次，編寫一個main函數m，讓它來調用函數pt：

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

最後，準備環境，讓ld都知道這5個路徑：

```sh 
(a) 往/etc/ld.so.conf總增加一行，內容：/tmp/st/3，然後執行 ldconfig 命令

(b) export LD_LIBRARY_PATH=/tmp/st/2
```

另外3中路徑，ld都可以得到，請接著看下面。

之後測試：

```sh
gcc m.c -o m1 -L/tmp/st/1 -lpt -Wl,-rpath,/tmp/st/1
./m1
```

```sh
start....

1  path on the gcc give

......end
```

這裡在可執行文件中動態段中DT_RPATH所指定的路徑，因此需要在編譯m.c的時候就指定路徑，由於其他路徑都也告訴了ld，很明顯，此種方法優先級最高了。

```sh
gcc m.c -o m -L/tmp/st/1 -lpt
./m
```

```sh
start....

2 path on the LD_LIBRARY_PATH

......end
```

這裡很顯然調用了LD_LIBRARY_PATH指定了路徑中的共享庫，因此此種情況優先級第二。

```sh
mv /tmp/st/2/libpt.so /tmp/st/2/libpt2.so
./m
```

```sh
start....

3 path on the /etc/ld.so.conf

......end
```

這裡是調用了/etc/ld.so.cache中所緩存的動態庫路徑中的共享庫，因此此種情況優先級第三。

```sh
mv /tmp/st/3/libpt.so /tmp/st/3/libpt3.so
./m
```

```sh
start....

4 path on the /lib

......end
```

這裡是調用/lib中的共享庫，優先級第四。

```sh
rm /lib/libpt.so
./m
```

```sh
start....

5 path on the /usr/lib

......end
```
這裡是調用/lib中的共享庫，優先級第五。

故證明這五種路徑指定方法的優先級是1-->2-->3-->4-->5！

---

一般我們在Linux下執行某些外部程序的時候可能會提示找不到共享庫的錯誤, 比如:

 

tmux: error while loading shared libraries: libevent-1.4.so.2: cannot open shared object file: No such file or directory


原因一般有兩個, 一個是操作系統裡確實沒有包含該共享庫(lib*.so.*文件)或者共享庫版本不對, 遇到這種情況那就去網上下載並安裝上即可. 

另外一個原因就是已經安裝了該共享庫, 但執行需要調用該共享庫的程序的時候, 程序按照默認共享庫路徑找不到該共享庫文件. 

所以安裝共享庫後要注意共享庫路徑設置問題, 如下:

###1) 如果共享庫文件安裝到了/lib或/usr/lib目錄下, 那麼需執行一下ldconfig命令

ldconfig命令的用途, 主要是在默認搜尋目錄(/lib和/usr/lib)以及動態庫配置文件/etc/ld.so.conf內所列的目錄下, 搜索出可共享的動態鏈接庫(格式如lib*.so*), 進而創建出動態裝入程序(ld.so)所需的連接和緩存文件. 緩存文件默認為/etc/ld.so.cache, 此文件保存已排好序的動態鏈接庫名字列表. 

###2) 如果共享庫文件安裝到了/usr/local/lib(很多開源的共享庫都會安裝到該目錄下)或其它"非/lib或/usr/lib"目錄下, 那麼在執行ldconfig命令前, 還要把新共享庫目錄加入到共享庫配置文件/etc/ld.so.conf中, 如下:

```sh
# cat /etc/ld.so.conf
include ld.so.conf.d/*.conf
# echo "/usr/local/lib" >> /etc/ld.so.conf
# ldconfig
```

### 3) 如果共享庫文件安裝到了其它"非/lib或/usr/lib" 目錄下,  但是又不想在/etc/ld.so.conf中加路徑(或者是沒有權限加路徑). 那可以export一個全局變量LD_LIBRARY_PATH, 然後運行程序的時候就會去這個目錄中找共享庫. 

LD_LIBRARY_PATH的意思是告訴loader在哪些目錄中可以找到共享庫. 可以設置多個搜索目錄, 這些目錄之間用冒號分隔開. 比如安裝了一個mysql到/usr/local/mysql目錄下, 其中有一大堆庫文件在/usr/local/mysql/lib下面, 則可以在.bashrc或.bash_profile或shell里加入以下語句即可:

```sh
export LD_LIBRARY_PATH=/usr/local/mysql/lib:$LD_LIBRARY_PATH    
```

一般來講這只是一種臨時的解決方案, 在沒有權限或臨時需要的時候使用.

--End--

---

假設在math目錄下已編輯好add.c sub.c div.c mul.c func_point.c文件，func_point.c為包含main（）的源文件！
動態庫的製作：

###方法一：

```sh
gcc -c -fPIC add.c sub.c div.c mul.c //-c表示生成.o目標文件,-f後加一些編譯選項，PIC表示與位置無關
gcc -shared -o libmymath.so add.o sub.o mul.o div.o//創建共享庫mymath，添加add.o,sub.o,mul.o,div.o目標文件
sudo mv libmymath.so /usr/lib
gcc func_point.c -lmymath//-l後面加動態鏈接庫名字
```

###方法二：

```sh
gcc -c -fPIC add.c sub.c div.c mul.c //-c:生成.o目標文件,-f後加一些編譯選項，PIC表示與位置無關
gcc -shared -o libmymath.so add.o sub.o mul.o div.o//創建共享庫mymath，添加add.o，sub.o mul.o div.o目標文件
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.//注意此操作後把當前目錄加入環境變量中
gcc func_point.c -L. -lmymath//-l後面加動態鏈接庫名字
```

###方法三：
```sh
gcc -c -fPIC add.c sub.c div.c mul.c //-c:生成.o目標文件,-f後加一些編譯選項，PIC表示與位置無關
gcc -shared -o libmymath.so add.o sub.o mul.o div.o//創建共享庫mymath，添加add.o，sub.o mul.o div.o目標文件
sudo vi /etc/ld.so.conf//把你的動態庫路徑加入此文件中
sudo ldconfig
gcc func_point.c -L. -lmymath//-l後面加動態鏈接庫名字
```

