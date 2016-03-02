# Linux內核kernel panic機制淺析

1.簡介：

內核錯誤(Kernel panic)是指操作系統在監測到內部的致命錯誤，並無法安全處理此錯誤時採取的動作。一旦到這個情況，kernel就儘可能把它此時能獲取的全部信息都打印出來，至於能打印出多少信息，那就看是哪種情況導致它panic了。`這個概念主要被限定在Unix以及類Unix系統中；對於Microsoft Windows系統，等同的概念通常被稱為藍屏死機`。


操作系統內核中處理Kernel panic的子程序（在AT&T派生類以及BSD類Unix中，通常稱為panic()）通常被設計用來向控制檯輸出錯誤信息，向磁盤保存一份內核內存的轉儲，以便事後的調試，然後等待系統被手動重新引導，或自動重新引導。該程序提供的技術性信息通常是用來幫助系統管理員或者軟件開發者診斷問題的。

操作系統試圖讀寫無效或不允許的內存地址是導致內核錯誤的一個常見原因。內核錯誤也有可能在遇到硬件錯誤或操作系統BUG時發生。在許多情況中，操作系統可以在內存訪問違例發生時繼續運行。然而，系統處於不穩定狀態時，操作系統通常會停止工作以避免造成破壞安全和數據損壞的風險，並提供錯誤的診斷信息。內核錯誤在早期的Unix系統中被引入，顯示了在Unix與Multics在設計哲學上的主要差異之一。Multics的開發者Tom van Vleck曾引述了一段在這個問題上與Unix開發者Dennis Ritchie的討論：

我提醒Dennis說，我在Multics中寫的近半數代碼都是錯誤恢復代碼。 他說：“我們不需要這些。我們有稱為panic的子程序，如果發生了錯誤就可以調用這個函數，使得系統崩潰， 然後你可以在大廳裡面大叫：‘嘿，重啟機器’。”

原始的`panic()`函數從UNIX第五版開始到基於VAX的UNIX 32V期間幾乎沒有變化，只是輸出一條錯誤信息，然後就使系統進入NOP的死循環中。當改進UNIX的基礎代碼的時候，panic()函數也有所改進，可以向控制檯輸出多種格式的調試信息。



kernel panic 主要有以下幾個出錯提示：

```sh
Kernel panic - not syncing fatal exception in interrupt
kernel panic - not syncing: Attempted to kill the idle task!
kernel panic - not syncing: killing interrupt handler!
Kernel Panic - not syncing：Attempted to kill init!
```

2.什麼能導致kernel panic：

一般出現下面的情況，就認為是發生了kernel panic:

- 機器徹底被鎖定，不能使用

- 數字鍵(Num Lock)，大寫鎖定鍵(Caps Lock)，滾動鎖定鍵(Scroll Lock)不停閃爍。

- 如果在終端下，應該可以看到內核dump出來的信息（包括一段”Aieee”信息或者”Oops”信息）

- 和Windows藍屏相似

有兩種主要類型kernel panic：

```sh
hard panic(也就是Aieee信息輸出)

soft panic (也就是Oops信息輸出)
```

只有`加載到內核空間的驅動模塊`才能`直接`導致kernel panic，你可以在系統正常的情況下，使用lsmod查看當前系統加載了哪些模塊。 除此之外，`內建在內核裡的組件`（比如memory map等）也能導致panic。因為hard panic和soft panic本質上不同，因此我們分別討論。


### 2.1 hard panic

原因

`對於hard panic而言，最大的可能性是驅動模塊的中斷處理(interrupt handler)導致的`，`一般是因為驅動模塊在中斷處理程序中訪問一個空指針(null pointre)`。一旦發生這種情況，驅動模塊就無法處理新的中斷請求，最終導致系統崩潰。

例如：在多核系統中，包括AP應用處理器、mcu微控制器和modem處理器等系統中，mcu控制器用於系統的低功耗控制，mcu微控制器由於某種原因超時向AP應用處理器發送一個超時中斷，AP接受中斷後調用中斷處理函數讀取mcu的狀態寄存器，發現是mcu的超時中斷，就在中斷處理程序中主動引用一個空指針，迫使AP處理器打印堆棧信息然後重啟linux系統。這就是一個典型的hard panic。

信息收集

根據panic的狀態不同，內核將記錄所有在系統鎖定之前的信息。因為kenrel panic是一種很嚴重的錯誤，不能確定系統能記錄多少信息，下面是一些需要收集的關鍵信息，他們非常重要，因此儘可能收集全，當然如果系統啟動的時候就kernel panic，那就無法知道能收集到多少有用的信息了。

- `/var/log/messages`: 幸運的時候，整個kernel panic棧跟蹤信息都能記錄在這裡。

- 應用程序/庫日誌: 可能可以從這些日誌信息裡能看到發生panic之前發生了什麼。

- 其他發生panic之前的信息，或者知道如何重現panic那一刻的狀態

- 終端屏幕dump信息，一般OS被鎖定後，複製，粘貼肯定是沒戲了，因此這類信息，你可以需要藉助數碼相機或者原始的紙筆工具了。

實際上，當內核發生panic時，linux系統會默認立即重啟系統，當然這只是默認情況，除非你修改了產生panic時重啟定時時間，這個值默認情況下是0，即立刻重啟系統。所以當panic時沒有把kernel信息導入文件的話，那麼可能你很難再找到panic產生的地方。


Linux的穩定性勿容置疑，但是有些時候一些Kernel的致命錯誤還是會發生（有些時候甚至是因為硬件的原因或驅動故障），`Kernel Panic會導致系統crash，並且默認的系統會一直hung在那裡，直到你去把它重新啟動！ 不過你可以在/etc/sysctl.conf文件中加入`

### 2.2 soft panic(oops)

在Linux上，oops即Linux內核的行為不正確，併產生了一份相關的錯誤日誌。`許多類型的oops會導致kernel panic，即令系統立即停止工作，但部分oops也允許繼續操作，作為與穩定性的妥協。這個概念只代表一個簡單的錯誤。`

`當內核檢測到問題時，它會打印一個oops信息然後終止全部相關進程`。oops信息可以幫助Linux內核工程師調試，檢測oops出現的條件，並修復導致oops的程序錯誤。
Linux官方內核文檔中提到的oops信息被放在內核源代碼Documentation/oops-tracing.txt中。通常klogd是用來將oops信息從內核緩存中提取出來的，然而，在某些系統上，例如最近的Debian發行版中，rsyslogd代替了klogd，因此，缺少klogd進程並不能說明log文件中缺少oops信息的原因。
若系統遇到了oops，一些內部資源可能不再可用。即使系統看起來工作正常，非預期的副作用可能導致活動進程被終止。oops常常導致kernel panic，若系統試圖使用被禁用的資源。Kernelloops提到了一種用於收集和提交oops到 http://www.kerneloops.org/ 的軟件 。Kerneloops.org同時也提供oops的統計信息。


症狀：
- 1.沒有hard panic嚴重
- 2.`通常導致段錯誤(segmentation fault)`
- 3.`可以看到一個oops信息，/var/log/messages裡可以搜索到’Oops’`
- 4.機器稍微還能用（但是收集信息後，應該重啟系統）

原因:
`凡是非中斷處理引發的模塊崩潰都將導致soft panic。在這種情況下，驅動本身會崩潰，但是還不至於讓系統出現致命性失敗，因為它沒有鎖定中斷處理例程`。導致hard panic的原因同樣對soft panic也有用（比如在運行時訪問一個空指針）

信息收集：
`當soft panic發生時，內核將產生一個包含內核符號(kernel symbols)信息的dump數據，這個將記錄在/var/log/messages裡`。為了開始排查故障，可以使用ksymoops工具來把內核符號信息轉成有意義的數據。
為了生成ksymoops文件,需要：
從/var/log/messages裡找到的堆棧跟蹤文本信息保存為一個新文件。確保刪除了時間戳(timestamp)，否則ksymoops會失敗。
運行ksymoops程序（如果沒有，請安裝）
詳細的ksymoops執行用法，可以參考ksymoops(8)手冊。

## 3.Kernel panic實例:

今天就遇到 一個客戶機器內核報錯：“Kernel panic-not syncing fatal exception”，重啟後正常，幾個小時後出現同樣報錯，系統down了，有時重啟後可恢復有時重啟後仍然報同樣的錯誤。

`什麼是fatal exception?`

`“致命異常（fatal exception）表示一種例外情況，這種情況要求導致其發生的程序關閉。通常，異常（exception）可能是任何意想不到的情況（它不僅僅包括程序錯誤）。致命異常簡單地說就是異常不能被妥善處理以至於程序不能繼續運行。`

`軟件應用程序通過幾個不同的代碼層與操作系統及其他應用程序相聯繫。當異常（exception）在某個代碼層發生時，為了查找所有異常處理的代碼，各個代碼層都會將這個異常發送給下一層，這樣就能夠處理這種異常。如果在所有層都沒有這種異常處理的代碼，致命異常（fatal exception）錯誤信息就會由操作系統顯示出來。這個信息可能還包含一些關於該致命異常錯誤發生位置的祕密信息（比如在程序存儲範圍中的十六進制的位置）。這些額外的信息對用戶而言沒有什麼價值，但是可以幫助技術支持人員或開發人員調試程序。`



`當致命異常（fatal exception）發生時，操作系統沒有其他的求助方式只能關閉應用程序，並且在有些情況下是關閉操作系統本身。當使用一種特殊的應用程序時，如果反覆出現致命異常錯誤的話，應將這個問題報告給軟件供應商。 ” 而且此時鍵盤無任何反應，必然使用reset鍵硬重啟。`

處理panic後的系統自動重啟panic.c源文件有個方法，當panic掛起後，指定超時時間，可以重新啟動機器，這就是前面說的panic超時重啟。

- vi linux-2.6.31.8/kernel/panic.c 

```c

if (panic_timeout > 0)
{
    /*
    * Delay timeout seconds before rebooting the machine.
    * We can't use the "normal" timers since we just panicked.
    */
    int i;
    printk(KERN_EMERG "Rebooting in %d seconds..", panic_timeout);

    for (i = 0; i < panic_timeout * 1000;) {
        touch_nmi_watchdog();
        i += panic_blink(i);
        mdelay(1);
        i++;
    }

    /*
    * This will not be a clean reboot, with everything
    * shutting down. But if there is a chance of
    * rebooting the system it will be rebooted.
    */
    emergency_restart();
} ...
```

### 修改方法：

```sh
/etc/sysctl.conf文件中加入
kernel.panic = 30 #panic錯誤中自動重啟，等待時間為30秒
kernel.sysrq=1 #激活Magic SysRq！ 否則，鍵盤鼠標沒有響應
```

設置kernel.sysrq=1使得鼠標鍵盤有反應了之後按住 [ALT]+[SysRq]+[COMMAND], 這裡SysRq是Print SCR鍵，而COMMAND按以下來解釋:

```sh
b – 立即重啟
e – 發送SIGTERM給init之外的系統進程
o – 關機
s – sync同步所有的文件系統
u – 試圖重新掛載文件系統
```

很多網友安裝linux出現“Kernel panic-not syncing fatal exception in interrupt”是由於網卡驅動原因。解決方法：將選項“Onboard Lan”的選項“Disabled”,重啟從光驅啟動即可。等安裝完系統之後，再進入BIOS將“Onboard Lan”的選項給“enable”，下載相應的網卡驅動安裝。
如出現以下報錯：
init() r8168 … 
… …
… ：Kernel panic: Fatal exception
r8168是網卡型號。
在BIOS中禁用網卡，從光驅啟動安裝系統。再從網上下載網卡驅動安裝。

```sh
#tar  vjxf  r8168-8.014.00.tar.bz2
# make  clean  modules       (as root or with sudo)
# make  install
# depmod  -a
# modprobe  r8168
```

安裝好系統後reboot進入BIOS把網卡打開。

另有網友在Kernel panic出錯信息中看到“alc880”，這是個聲卡類型。嘗試著將聲卡關閉，重啟系統，搞定。

安裝linux系統遇到安裝完成之後，無法啟動系統出現Kernel panic-not syncing fatal exception。很多情況是由於板載聲卡、網卡、或是cpu 超線程功能（Hyper-Threading ）引起的。這類問題的解決辦法就是先查看錯誤代碼中的信息，找到錯誤所指向的硬件，將其禁用。系統啟動後，安裝好相應的驅動，再啟用該硬件即可。

另外出現`“Kernel Panic — not syncing: attempted to kill init”和“Kernel Panic — not syncing: attempted to kill idle task”`有時把內存互相換下位置或重新插拔下可以解決問題。

還有一種情況，swap交換分區沒有配置的時候，也會出現”kernel panic – not syncing : attempted to kill init”，已在RHEL6.2_64bit上測試。

##參考：
http://blog.csdn.net/ylyuanlu/article/details/9115159
http://wiki.linuxdeepin.com/index.php?title=Linux_kernel_panic&oldid=7764
http://www.vpsee.com/2010/08/reboot-linux-after-a-kernel-panic/
http://www.4byte.cn/question/721620/how-to-detect-a-kernel-panic-on-a-remote-machine.html

