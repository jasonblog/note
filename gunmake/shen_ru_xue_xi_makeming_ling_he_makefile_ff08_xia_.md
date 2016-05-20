# 深入學習Make命令和Makefile（下）


make是Linux下的一款程序自動維護工具，配合makefile的使用，就能夠根據程序中模塊的修改情況，自動判斷應該對那些模塊重新編譯，從而保證軟件是由最新的模塊構成。 
本文分為上下兩部分，我們在上一篇文章中分別介紹了make和makefile的一些基本用法，在本文中，我們會對make和makefile的功能做進一步的介紹。

##一、構建多個目標

有時候，我們想要在一個makefile中生成多個單獨的目標文件，或者將多個命令放在一起，比如，在下面的示例mymakefile3中我們將添加一個clean 選項來清除不需要的目標文件，然後用install選項將生成的應用程序移動到另一個目錄中去。這個makefile跟前面的mymakefile較為相似，不同之處筆者用黑體加以標識：


```sh
all: main
# 使用的編譯器
CC = gcc
# 安裝位置
INSTDIR = /usr/local/bin
# include文件所在位置
INCLUDE = .
# 開發過程中所用的選項
CFLAGS = -g -Wall –ansi
# 發行時用的選項
# CFLAGS = -O -Wall –ansi

main: main.o f1.o f2.o
    $(CC) -o main main.o f1.o f2.o
main.o: main.c def1.h
    $(CC) -I$(INCLUDE) $(CFLAGS) -c main.c
f1.o: f1.c def1.h def2.h
    $(CC) -I$(INCLUDE) $(CFLAGS) -c f1.c
f2.o: f2.c def2.h def3.h
    $(CC) -I$(INCLUDE) $(CFLAGS) -c f2.c

clean:
    -rm main.o f1.o f2.o
install: main
@if [ -d $(INSTDIR) ]; \
then \
cp main $(INSTDIR);\
chmod a+x $(INSTDIR)/main;\
chmod og-w $(INSTDIR)/main;\
echo “Installed in $(INSTDIR)“;\
else \
echo “Sorry, $(INSTDIR) does not exist”;\
fi
```
在這個makefile中需要注意的是，雖然這裡有一個特殊的目標all，但是最終還是將main作為目標。因此，如果執行make命令時沒有在命令行中給出一個特定目標的話，仍然會編譯連接main程序。 

其次要注意後面的兩個目標：clean和install。目標clean沒有依賴模塊，因為沒有時間標記可供比較，所以它總被執行；它的實際意圖是引出後面的rm命令來刪除某些目標文件。我們看到rm命令以-開頭，這時即使表示make將忽略命令結果，所以即使沒有目標供rm命令刪除而返回錯誤時，make clean依然繼續向下執行。 

接下來的目標install依賴於main，所以make知道必須在執行安裝命令前先建立main。用於安裝的指令由一些shell命令組成。 

因為make調用shell來執行規則，並且為每條規則生成一個新的shell，所以要用一個shell來執行這些命令的話，必須添加反斜槓，以使所有命令位於同一個邏輯行上。這條命令用@開頭，表示在執行規則前不會向標準輸出打印命令。 
為了安裝應用程序，目標install會一條接一條地執行若干命令，並且執行下一個之前，不會檢查上一條命令是否成功。若想只有當前面的命令取得成功時，隨後的命令才得以執行的話，可以在命令中加入&&，如下所示：


```sh
@if [ -d $(INSTDIR) ]; \
then \
cp main $(INSTDIR) &&\
chmod a+x $(INSTDIR)/main && \
chmod og-w $(INSTDIR/main && \
echo “Installed in $(INSTDIR)“ ;\
else \
echo “Sorry, $(INSTDIR) does not exist” ; false ; \
fi
```

這是shell的“與”指令，只有當在前的命令成功時隨後的命令才被執行。這裡不必關心前面命令是否取得成功，只需注意這種用法就可以了。 

要想在/usr/local/bin目錄安裝新命令必須具有特權，所以調用make install命令之前，可以讓Makefile使用一個不同的安裝目錄，或者修改該目錄的權限，或切換到root用戶。如下所示：



```sh
$ rm *.o main
$ make -f Mymakefile3
gcc -I. -g -Wall -ansi -c main.c
gcc -I. -g -Wall -ansi -c f1.c
gcc -I. -g -Wall -ansi -c f2.c
gcc -o main main.o f1.o f2.o
$ make -f Mymakefile3
make: Nothing to be done for ‘all’.
$ rm main
$ make -f Mymakefile3 install
gcc -o main main.o f1.o f2.o
Installed in /usr/local/bin
$ make -f Mymakefile3 clean
rm main.o f1.o f2.o
$
```

讓我們對此作一簡單介紹，首先刪除main和所有目標文件程序，由於將all作為目標，所以make命令會重新編譯main。當我們再次執行make命令時，由於main是最新的，所以make什麼也不做。之後，我們刪除main程序文件，並執行make install，這會重新建立二進制文件main並將其複製到安裝目錄。最後，運行make clean命令，來刪去所有目標程序。

##二、內部規則

迄今為止，我們已經能夠在makefile中給出相應的規則來指出具體的處理過程。實際上，除了我們顯式給出的規則外，make還具有許多內部規則，這些規則是由預先規定的目標、依賴文件及其命令組成的相關行。在內部規則的幫助下，可以使makefile變得更加簡潔，尤其是在具有許多源文件的時候。現在以實例加以說明，首先建立一個名為foo.c的C程序源文件，文件內容如下所示：

```c
#include <stdio.h>
int main()
{
    printf(“Hello World\n”);
    exit(EXIT_SUCCESS);
}
```

現在讓我們用make命令來編譯它：

```c
$ make foo
cc foo.c -o foo
$
```

您會驚奇地發現，儘管我們沒有指定makefile，但是make仍然能知道如何調用編譯器，並且調用的是cc而不是gcc編譯器。這在Linux上沒有問題，因為cc常常會鏈接到gcc程序。這完全得益於make內建的內部規則，另外這些內部規則通常使用宏，所以只要為這些宏指定新的值，就可以改變內部規則的默認動作，如下所示：

```c
$ rm foo
$ make CC=gcc CFLAGS=”-Wall -g” foo
gcc -Wall -g foo.c -o foo
$
```

用make命令加-p選項後，可以打印出系統缺省定義的內部規則。它們包括系統預定義的宏、以及產生某些種類後綴的文件的內部相關行。內部規則涉及的文件種類很多，它不僅包括C源程序文件及其目標文件，還包括SCCS文件、yacc文件和lex文件，甚至還包括Shell文件。 
當然，我們更關心的是如何利用內部規則來簡化makefile，比如讓內部規則來負責生成目標，而只指定依賴關係，這樣makefile就簡潔多了，如下所示：


```c
main.o: main.c def1.h
f1.o: f1.c def1.h def2.h
f2.o: f2.c def2.h def3.h
```

### 三、後綴規則

前面我們已經看到，有些內部規則會根據文件的後綴（相當於Windows系統中的文件擴展名）來採取相應的處理。換句話說，這樣當make見到帶有一種後綴的文件時，就知道使用哪些規則來建立一個帶有另外一種後綴的文件，最常見的是用以.c結尾的文件來建立以.o結尾的文件，即把源文件編譯成目標程序，但是不連接。


現在舉例說明後綴規則的應用。有時候，我們需要在不同的平臺下編譯源文件，例如Windows和Linux。假設我們的源代碼是C++編寫的，那麼Windows下其後綴則為.cpp。不過Linux使用的make版本沒有編譯.cpp文件的內部規則，倒是有一個用於.cc的規則，因為在UNIX操作系統中c++文件擴展名通常為.cc。


這時候，要麼為每個源文件單獨指定一條規則，要麼為make建立一條新規則，告訴它如何用.cpp為擴展名的源文件來生成目標文件。如果項目中的源文件較多的話，後綴規則就可以派上用場了。要添加一條新後綴規則，首先在makefile文件中加入一行來告訴make新後綴是什麼；然後就可以添加使用這個新後綴的規則了。這時，make要用到一條專用的語法：


```sh
.<舊後綴名>.<新後綴名>:
```

它的作用是定義一條通用規則，用來將帶有舊後綴名的文件變成帶有新後綴名的文件，文件名保持不變，如要將.cpp文件編譯成.o文件，可以使用一個新的通用規則：

```sh
.SUFFIXES: .cpp
.cpp.o:
$(CC) -xc++ $(CFLAGS) -I$(INCLUDE) -c $<
```

上面的.cpp .o:告訴make 這些規則用於把後綴為.cpp的文件轉換成後綴為.o的文件。其中的標誌-xc++的作用是告訴gcc這次要編譯的源文件是c++源文件。這裡，我們使用一個宏$<來通指需要編譯的文件的名稱，不管這些文件名具體是什麼。我們只需知道，所有以.cpp為後綴的文件將被編譯成以.o為後綴的文件，例如以是app.cpp的文件將變成app.o。


注意，我們只跟make說明如何把.cpp文件變成.o文件就行了，至於如何從目標程序文件變成二進制可執行文件，因為make早已知曉，所以就不用我們費心了。所以，當我們調用make程序時，它會使用新規則把類似app .cpp這樣的程序變成app.o，然後使用內部規則將app.o文件連接成一個可執行文件app。

現在，make已經知道如何處理擴展名為.cpp的c++源文件，除此之外，我們還可以通過後綴規則將文件從一種類型轉換為另一種類型。不過，較新版本的make包含一個語法可以達到同樣的效果。例如，模式規則使用%作為匹配文件名的通配符，而不單獨依賴於文件擴展名。以下模式規則相當於上面處理.cpp的規則，具體如下所示：

```sh
%.cpp: %o
$(CC) -xc++ $(CFLAGS) -I$(INCLUDE) -c $<
```

## 四、用make管理程序庫
一般來說，程序庫也是一種由一組目標程序構成的以.a為擴展名的文件，所以，Make命令也可以用來管理這些程序庫。實際上，為了簡化程序庫的管理，make程序還專門設有一個語法：

```sh
lib (file.o)
```

這意味著目標文件file.o以庫文件lib.a的形式存放，這意味著lib.a庫依賴於目標程序file.o。此外，make命令還具有一個內部規則用來管理程序庫，該規則相當於如下內容：

```sh
.c.a:
$(CC) -c $(CFLAGS) $<
$(AR) $(ARFLAGS) $@ $*.o
```

其中，宏$(AR)和$(ARFLAGS)分別表示指令AR和選項rv。如上所見，要告訴make用.c文件生成.a庫，必須用到兩個規則：第一個規則是說把源文件編譯成一個目標程序文件。第二個規則表示使用ar 指令向庫中添加新的目標文件。

所以，如果我們有一個名為filed的庫，其中含有bar.o文件，那麼第一規則中的$<會被替換為bar.c；在第二個規則中的$@被庫名filed.a所替代，而$*將被bar所替代。

下面舉例說明如何用make來管理庫。實際上，用make來管理程序庫的規則是很簡單的。比如，我們可以將前面示例加以修改，讓f1.o和f2.o放在一個稱為mylib.a的程序庫中，這時的Makefile幾乎無需改變，而新的mymakefile4看上去是這樣的：

```sh
all: main
# 使用的編譯器
CC = gcc
# 安裝位置
INSTDIR = /usr/local/bin
# include文件所在位置
INCLUDE = .
# 開發過程中使用的選項
CFLAGS = -g -Wall –ansi
# 用於發行時的選項
# CFLAGS = -O -Wall –ansi
# 本地庫
MYLIB = mylib.a

main: main.o $(MYLIB)
    $(CC) -o main main.o $(MYLIB)
    $(MYLIB): $(MYLIB)(f1.o) $(MYLIB)(f2.o)
    main.o: main.c def1.h
    f1.o: f1.c def1.h def2.h
    f2.o: f2.c def2.h def3.h

clean:
    -rm main.o f1.o f2.o $(MYLIB)
install: main
@if [ -d $(INSTDIR) ]; \
then \
cp main $(INSTDIR);\
chmod a+x $(INSTDIR)/main;\
chmod og-w $(INSTDIR)/main;\
echo “Installed in $(INSTDIR)“;\
else \
echo “Sorry, $(INSTDIR) does not exist”;\
fi
```
注意：我們是如何讓省缺規則來替我們完成大部分工作的。如今，我們可以試一下新版的makefile的工作情況：


```sh
$ rm -f main *.o mylib.a

$ make -f Mymakefile4
gcc -g -Wall -ansi -c -o main.o main.c
gcc -g -Wall -ansi -c -o f1.o f1.c
ar rv mylib.a f1.o
a - f1.o
gcc -g -Wall -ansi -c -o f2.o f2.c
ar rv mylib.a f2.o
a - f2.o
gcc -o main main.o mylib.a

$ touch def3.h

$ make -f Mymakefile4
gcc -g -Wall -ansi -c -o f2.o f2.c
ar rv mylib.a f2.o
r - f2.o
gcc -o main main.o mylib.a
$
```
現在對上面的例子做必要的說明。首先刪除全部目標程序文件和程序庫，然後讓make重新構建main，因為當連接main.o時需要用到庫，所以要先編譯和創建庫。此後，我們還測試f2.o的依賴關係，我們知道如果def3.h發生了改變，那麼必須重新編譯f2.c，事實表明make在重新構建main可執行文件之前，正確地編譯了f2.c並更新了庫。


## 五、Makefile和子目錄

如果你的項目比較大的話，可以考慮將某些文件組成一個庫，然後單獨存放到一個子目錄內。這時，對於makefile有兩種處理方法，下面分別介紹。

第一種方法：在子目錄中放置一個輔助makefile，然後把這個子目錄中的源文件編譯成一個程序庫，最後將這個庫複製到主目錄中。上級目錄中的主要makefile可以放上一個規則，通過調用輔助makefile來建立該庫：

```sh
mylib.a:
(cd mylibdirectory;$(MAKE))
```
這樣的話，我們就會總是構建mylib.a，因為冒號右邊為空。當make調用該規則構建該庫時，它會切換到子目錄mylibdirectory中，然後調用一個新的make命令來管理該庫。因為調用了一個新的shell來完成此任務，所以使用makefile的程序不必進行目錄切換。不過，被調用的shell是在一個不同的目錄中利用該規則構建該庫的，所以括弧能確保所有處理都是由一個shell完成的。

第二種方法：在單個makefile中使用更多的宏，不過這些附加的宏需要在目錄名上加D並且為文件名加上F。例如，可以用下面的命令來覆蓋內建的.c.o後綴規則：

```sh
.c.o:
$(CC) $(CFLAGS) -c $(@D)/$(
```

為在子目錄編譯文件，並將目標放在子目錄中，可以用像下面這樣的依賴關係和規則來更新當前目錄中的庫：

```sh
mylib.a: mydir/f1.o mydir/f2.o
ar -rv mylib.a $?
```

上述兩種方法都是可行的，至於使用哪一種，需要根據您的項目的具體情況來決定。


## 六、GNU make和gcc的有關選項

如果您當前正在使用GNU make和GNU gcc編譯器的話，那麼它們還分別有一個額外的選項可以使用，下面分別加以說明。

我們首先介紹用於make程序的-jN 選項。這個選項允許make同時執行N條命令。這樣的話，就可以將該項目的多個部分單獨進行編譯，make將同時調用多個規則。如果具有許多源文件的話，這樣做能夠節約大量編譯時間。

其次，gcc還有一個-MM選項可用，該選項會為make生成一個依賴關係表。在一個含有大量源文件的項目中，很可能每個源文件都包含一組頭文件，而頭文件有時又會包含其它頭文件，這時正確區分依賴關係就比較難了。這時為了防止遺漏，最笨的方法就是讓每個源文件依賴於所有頭文件，但這顯然沒有必要；另一方面，如果你遺漏一些依賴關係的話，就根本就無法編譯通過。這時，我們就可以用gcc的-MM選項來生成一張依賴關係表，例如：

```sh
$ gcc -MM main.c f1.c f2.c
main.o: main.c def1.h
f1.o: f1.c def1.h def2.h
f2.o: f2.c def2.h def3.h
$
```

這時，gcc編譯器會掃描所有源文件，並生產一張滿足makefile格式要求的依賴關係表，我們只須將它保存到一個臨時文件內，然後將其插入makefile即可。

##七、小結

繼上一篇文章之後，本文又對make和makefile的一些高級應用作了相應的介紹，至此，我們已經對make和makefile在程序開發中的應用有了一個較為全面的認識，希望本文能對讀者的學習和工作有所幫助。
