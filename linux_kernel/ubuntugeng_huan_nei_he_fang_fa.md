# Ubuntu更換內核方法


目前，我的Ubuntu系統的內核版本是3.11.0，我想使用低版本的內核3.2.65來替代它，步驟如下：


以下過程全部在root權限下操作。

1、 安裝必備軟件編譯工具：

```sh
#apt-get install libncurses5-dev build-essential kernel-package
```

注意：

（1）libncurses5-dev是為之後配置內核能運行 make menuconfig程序做準備

Build-essential為編譯工具，kernel-package是編譯內核工具

（2）如果系統顯示無法查找到這三個文件，輸入#apt-get update更新數據源。

2、 下載內核

去www.kernel.org下載你需要的內核版本的源代碼：linux-3.2.65.tar.xz  解壓後linux-3.2.65.
4、 將該壓縮包解壓後的內容拷貝到/usr/src/目錄下
```sh
cp  -r  linux-3.2.65  /usr/src/
```
5、 配置內核：

(1)建議使用當前系統配置文件，使用當前系統配置文件方法：

```sh
#cp /boot/config-XX  ./.config，
```

(2)#make mrproper     ///這一步可以不做！
作用是在每次配置並重新編譯內核前需要先執行「make mrproper」命令清理源代碼樹，包括過去曾經配置的內核配置文件「.config」都將被清除。即進行新的編譯工作時將原來老的配置文件給刪除到，以免影響新的內核編譯。

即檢查有無不正確的.o文件和依賴關係，如果使用剛下載的完整的源程序包即第一次進行編譯，那麼本步可以省略。而如果你多次使用了這些源程序編譯內核，則最好要先運行一下這個命令。

(3)然後進入menuconfig選擇load選項。
```sh
#make menuconfig
```

註：使用make menuconfig 生成的內核配置文件，決定將內核的各個功能系統編譯進內核還是編譯為模塊還是不編譯。

在這裡就不介紹具體的內核配置操作，但是建議就算不打算配置什麼也執行一下make menuconfig這個命令，因為如果不執行此操作的話在後面make編譯內核的時候會提示你回答很多問題。

內核配置可參考：

Make menuconfig配置詳解：http://blog.csdn.net/xuyuefei1988/article/details/8635539

http://www.linuxidc.com/Linux/2012-06/63092.htm

配置內核還有很多其他方法，主要區別如下：

```sh
#make menuconfig //基於ncurse庫編制的圖形工具界面
#make config //基於文本命令行工具，不推薦使用
#make xconfig //基於X11圖形工具界面
#make gconfig //基於gtk+的圖形工具界面
```

這裡選擇簡單的配置內核方法，即make menuconfig。在終端輸入make menuconfig，等待幾秒後，終端變成圖形化的內核配置界面。進行配置時，大部分選項使用其缺省值，只有一小部分需要根據不同的需要選擇。

對每一個配置選項，用戶有三種選擇，它們分別代表的含義如下：
```sh
<*>或[*]——將該功能編譯進內核
[ ]——不將該功能編譯進內核
[M]——將該功能編譯成可以在需要時動態插入到內核中的代碼
```

6、 編譯內核：

```sh
#make         ///這可能需要2~3個小時的時間。
```

7、加入模塊

```sh
#make modules_install
```

8、生成可執行的內核引導文件

```sh
#make bzImage
```

這一步會生成內核引導文件，其路徑是： arch/x86/boot/bzImage
```sh
#cp arch/x86/boot/bzImage /boot/vmlinuz-3.2.65
```

這一步，是將剛剛生成的引導文件拷貝到 /boot的目錄下,並重新命名為vmlinuz-3.2.65
9、將System.map複製到/boot下：

```sh
#cp System.map /boot/System.map-3.2.65
```

10、生成initrd.img文件
```sh
#cd /lib/modules/3.2.65
#update-initramfs –c –k 3.2.65
```
11、自動查找新內核，並添加至grub引導
```sh
#update-grub
```

12、修改/boot/grub/grub.cfg文件，將幾個timeout的值設為10

這樣grub在引導的時候就會出現10秒的停留，如果有按鍵輸入，則可以對內核鏡像進行選擇.


還可以直接設置「set default="0」, 把0修改為對應的新內核對應的數值（查看新內核menuentry的序號，第一個是0，我的安裝完後排在第三個，所以序號是2） 

set default="2".不過修改默認啟動的系統要謹慎，別把序號搞錯了！！

OK， 至此，新的內核一切成功！重啟試試吧！