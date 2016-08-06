# 談談autotools 的 Build, Target, 和host

先來個小故事，某天組裝工趁放假跑去看老房子。發現只有一間上面有動物的造型。因為年代久遠，只看得出來有四肢腳而已，而不知道是那種動物。好奇之餘問了店家，店家說他們承租，不知道那是什麼東西。後來一位長者經過，就順便請教他同一個問題，長者說他住這邊好幾年都不知道上面有這東西。最後遇到當地文史工作室的人，問了才知道是當時主人的商標，一隻獅子。

我們常常用的東西，因為太習以為常，以至於發生視而不見的狀況屢見不鮮。但是惡魔就在細節裏面，而長時間地視而不見就會把自己對於這樣的事物感受性變低。

回到主體。組裝的時候，常常缺東缺西的。這時候就需要cross-compile一些套件頂一下，所以如果有人組裝過的話，應該會對下面的命令倍感親切。

```sh
$ ./configure --host=mipsel
```

如果組裝夠次數夠多，也會看到`--target`和`--build`的參數似乎和這個有關。找時間稍微了解一下

<font color="red">對於--target目前手上的資料無法讓我完全理解，請注意！</font>

根據[Autoconf手冊](https://www.gnu.org/software/autoconf/manual/autoconf-2.69/html_node/Hosts-and-Cross_002dCompilation.html#Hosts-and-Cross_002dCompilation)說明，這些參數的預設值如下:

* `--build`
  * 從config.guess中猜的
* `--host`
	* 設成和`--build`相同
* `--target`
	* 設成和`--host`相同

因為這樣的連動性，如果你要cross-compile，那麼下了`--host`後麻煩不要省掉`--build`，不然autotool會把build設成和host一樣。

Automake手冊裏面的定義

* `--build`
	* 你build code，下xxx-gcc那台機器平台
* `--host`
	* 產生的binary可以執行的平台
* `--target`
  * 告訴gcc要產生什麼平台的機械碼
	* 一般來說會和`--host`一樣同樣的平台，根據上面的連動，你設了`--host`就可以省略這個選項
	* 特例是compile cross-compiler或是binutils，toolchain。這時候是指定build在host上，但是處理的對象是cross platform的機械碼
  
Autotool選擇gcc的方式如下

* `--host`和`--build`相同
	* 用你電腦上的gcc，來編譯autotool的套件。
* `--host`和`--build`不同
	* 你要自備cross-compiler
	* 如果有指定`--target`的為gcc的prefix
  * 如`--host=xxx`, `--target=zzz`，會使用zzz-gcc產生xxx平台的機械碼
  * 用該cross-compiler來編譯autotool的套件
  * 這時候要保證cross compiler可以吃的library也存在，常常會為了編一個套件cross compile一堆套件
  
看的有點混亂嘛？沒關係，autoconf 2.69手冊有提到，要cross-compile的話，唯一(if and only if)的條件就是指令`--host`指定和你現在的平台不同就好。
  
## 參考資料

* [Autoconf手冊: Hosts and Cross-Compilation](https://www.gnu.org/software/autoconf/manual/autoconf-2.69/html_node/Hosts-and-Cross_002dCompilation.html)
* [What's the difference of ./configure option --build, --host and --target?](http://stackoverflow.com/questions/5139403/whats-the-difference-of-configure-option-build-host-and-target)
* [GCC online documentation: 6.1 Configure Terms and History](http://gcc.gnu.org/onlinedocs/gccint/Configure-Terms.html)
* [Automake手冊：2.2.8 Cross-Compilation](http://www.gnu.org/software/automake/manual/html_node/Cross_002dCompilation.htmlaut)
