---
title: Linux 硬件啟動
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,內核,kernel,編譯,啟動流程
description: 從內核加電之後，簡單介紹如何從硬件加載啟動。
---

從內核加電之後，簡單介紹如何從硬件加載啟動。

<!-- more -->

## Bootstrap

這裡的 [Bootstrap](http://en.wiktionary.org/wiki/bootstrap) 通常是指 "鞋子後邊的揹帶"，通常也表示不需要別人的 幫助就能夠自己處理事情的情形。這來自於一句常用的諺語 ```"pull oneself up by one's bootstraps"``` 。

這句話最早出現於 《The Surprising Adventures of Baron Munchausen》，這本書裡的一個故事：主人公 Baron Munchausen 不小心掉進了一片沼澤，他通過自己的 bootstraps 將自己拉了出來，Magic :-)

其實在 19 世紀初美國也有 ```"pull oneself over a fence by one's bootstraps"``` 的說法，意思是 "做荒謬不可能完成的事情" 。可參考 [Bootstrapping](http://en.wikipedia.org/wiki/Bootstrapping) 。

最早的時候，工程師們用它來比喻計算機啟動是一個很矛盾的過程：必須先運行程序，然後計算機才能啟動，但是計算機不啟動就無法運行程序！

早期真的是這樣，必須想盡各種辦法，把一小段程序裝進內存，然後計算機才能正常運行。所以，工程師們把這個過程叫做"拉鞋帶"，久而久之就簡稱為 boot 了。

接下來主要介紹的是現在最常用的 PC 啟動方法，也即採用 BIOS 啟動，通常還有其它的啟動方式，如 EFI、UEFI 等。

> 可擴展固件接口 (Extensible Firmware Interface，EFI) 是 Intel 為全新類型的 PC 固件的體系結構、接口和服務提出的建議標準。主要目的是為了提供一組在 OS 加載之前（啟動前）在所有平臺上一致的、正確指定的啟動服務，被看做是有近 20 多年曆史的 PC BIOS 的繼任者。
>
> 統一的可擴展固件接口 (Unified Extensible Firmware Interface，UEFI)，是一種詳細描述全新類型接口的標準，由 EFI 1.10 為基礎發展起來的，它的所有者已不再是 Intel，而是一個稱作 Unified EFI Form 的國際組織，是 open source 的。這種接口用於操作系統自動從預啟動的操作環境，加載到一種操作系統上，從而使開機程序化繁為簡，節省時間。

## 主板介紹

在介紹之前首先了解一下主板的情況。現在主板大致如下所示，在主板上主要有兩大主要部分：北橋 (North Bridge 或 Host Bridge) 和南橋 (South Bridge) 。

北橋主要負責 CPU 和內存、顯卡這些部件的數據傳送，而南橋主要負責 I/O 設備、外部存儲設備以及 BIOS 之間的通信。現在有些芯片廠商為了加快數據的傳輸速度，已經把北橋所負責的功能直接集成到 CPU 中了，因此有些主板已經沒有北橋了。

![hardware materboard diagram]({{ site.url }}/images/linux/kernel/hardware_motherboard_diagram.png "hardware materboard diagram"){: .pull-center }

CPU 通過管腳與外界相連，但是它不知道與之相連的是什麼，可能是主板、路由、測試工具等。 CPU 主要通過三種方式與外界通訊：內存地址空間、I/O地址空間和中斷。在主板中， CPU 通過前端總線 (Front Side Bus) 與北橋相連，其中包括了地址總線和數據總線。對於 Intel Core 2 QX6600 而言，有 33 針地址總線，64 針數據總線，因此 CPU 可訪問的地址空間有 64GB = 2^33 * 8B ，實際使用只是其中一部分。

在訪問上述的地址空間時，需要通過北橋與其他設備進行通訊，這種通訊方式稱為內存映射 ( [Memory Mapped I/O, MMIO](http://en.wikipedia.org/wiki/Memory-mapped_IO) ) 。若要使 MMIO 空間可用於 32 位操作系統，MMIO 空間必須駐留在前 4GB 地址空間內。這些地址空間一部分會映射到 RAM ，還有其他的設備，如顯卡、PCI 設備、BIOS 等。在 Linux 中，可以通過 /proc/iomem 查看，其分佈大致如下。

![hardware materboard diagram]({{ site.url }}/images/linux/kernel/hardware_memory_layout.png "hardware materboard diagram"){: .pull-center }

### 啟動流程

操作系統啟動大致過程如下圖所示，總共經過了四步：BIOS、MBR、Bootloader 和 操作系統初始化。

![hardware boot step]({{ site.url }}/images/linux/kernel/hardware_boot_steps.png "hardware boot step"){: .pull-center }

## 第一階段：BIOS

當按下開關時主板會按照一定的順序啟動各個芯片，當然不同的主板啟動的順序會不一樣。剛開始時電壓還不太穩定，主板上的控制芯片組會向 CPU 發出並保持一個 RESET 信號，讓 CPU 內部自動恢復到初始狀態，此時 CPU 在此刻不會馬上執行指令。當芯片組檢測到電源已經開始穩定供電了，它便撤去 RESET 信號（如果是手工按下計算機面板上的 Reset 按鈕來重啟機器，那麼鬆開該按鈕時芯片組就會撤去 RESET 信號），然後 CPU 開始運行。

![hardware bios hardware]({{ site.url }}/images/linux/kernel/hardware_bios_hardware.jpg "hardware bios hardware"){: .pull-center }

> 上個世紀70年代初，"只讀內存" (read-only memory，ROM) 發明，開機程序被刷入 ROM 芯片，計算機通電後，第一件事就是讀取它。這塊芯片裡的程序叫做 "基本輸入輸出系統" (Basic Input/Output System) ，簡稱為 [BIOS](http://en.wikipedia.org/wiki/BIOS) 。

如果是多處理器，通常會動態選擇一個 Bootstrap Processors(BSP) ，該 CPU 會執行所有的 BIOS 和內核初始化函數。其他的處理器 Application Processors (AP) 會一直等到內核明確激活之後才會運行。

> 在 8086 的那個時代，採用的是實模式(Real Mode)， CPU 尋址時採用 Segment:Offset 模式，物理地址為 Address = Segment * 16 + Offset ，此時段的大小固定為 2^16=64K ，最大的尋址為 0x00000 ~ 0xFFFFF ，即 1MBytes 。
>
> 80386 之後產生了保護模式 (Protect Mode) ，虛擬地址到物理地址轉換時需要通過兩步進行，即段機制 + 頁機制，其中段轉換機制是必須的。段機制在轉換時採用段描述符，使用 64bits 而不是 16bits 來描述一個段，包括了段基址、段長、屬性等，此時段的大小是可變的。
>
> 在保護模式中，可以單獨使用段機制；也可以使用頁機制，此是會分為兩步，首先經過段機制然後是分頁機制。
>
> DMI/QPI (Quick Path Interconnect，快速通道互聯)，是 Intel 用來取代 FSB 的新一代高速總線，對於 CPU 與 CPU 之間或者 CPU 與北橋芯片之間的通訊都可以使用 QPI 相連。

在 8086 初始化時 ```CS:IP=F000h:FFF0h``` ， 因此將會從 ```FFFF0h``` 開始執行。 80386 之後採用了一種 hack 的方式 (與實模式和保護模式都不太一樣) ，此時 ```CR0:PE``` 沒有置 1 ，因此不是保護模式，該模式被稱為 Big Real Mode。

在初始化時 EIP 仍然為 ```0000FFF0h```， CS 的 ```Selector=F000h```， ```Base=FFFF0000h```， ```Limit=FFFFh```， ```AR=Present, R/W, Accessed``` ，此時 GTD 還沒有建立，Selector 沒有使用，相應的讀取地址為 ```CS:Base+EIP=FFFFFFF0h``` 。

接下來不同的廠商做法就不太一樣了，主要是 Intel 設計的 EFI(Extensible Firmware Interface) 的做法和傳統的 Legacy BIOS 。

* Legacy BIOS<br>放在 0xFFFFFFF0 的第一條指令一般是一個遠跳轉指令 (Far Jump) ，也就是說 CPU 在執行 Legacy BIOS 時，會直接從 0xFFFFFFF0 跳回 F Segment ，回到 1MB 以下這個 Legacy BIOS 的老巢裡去。此時重裝 CS 後，上述的 hack 將無效，也就是到了 8086 時的實模式。此時可以訪問的地址範圍還和 A0 有關，沒有詳細瞭解 :(

* EFI BIOS<br>第一條指令是 wbinvd(清除 CPU 高速緩存)，之後做一些設定之後，會直接進入保護模式。所以 EFI BIOS 是從南橋 Region 4G 通過，並不需要 Legacy Region 。

下面僅對 Legacy BIOS 進行介紹。

對於傳統的 “CPU + 北橋 + 南橋” 類型的主板來說，CPU 的地址請求通過 FSB (Front Side BUS, 前端總線) 到達北橋，北橋通過判斷這個地址範圍在南橋，因此將這個請求送到南橋。

對於最新的主板芯片組來說，北橋和 CPU 封裝在一顆芯片裡面，所以會看到這個請求通過 DMI/QPI 被送到南橋。請求到達南橋後，南橋根據目前的地址映射表的設置決定是否將請求轉發到 SPI(Serial Peripheral Interface) 或者 LPC(Low Pin Count)。

簡單說就是南橋芯片擁有一張地址映射表，當有地址解析的請求到來時，南橋查看這張表決定將地址解析到何處去。這張表裡有兩個特殊區域，一個是從地址空間 4G 向下，大小從 4MB 到 16MB 不等的一個區域，我們以 4MB 為例，地址空間從 ```FFFC00000h~FFFFFFFFh``` 。稱之為 Range 4G 。第二個區域一般是是從 1MB 向下 128KB 的範圍，即 E Segment 和 F Segment ，從 ```E0000~FFFFF``` ，稱之為 Legacy Range ，也就是說， ```FFFC00000h~FFFFFFFFh``` 之間和 ```E0000~FFFFF``` 之間的尋址請求都會被導向到 SPI/LPC ，最終指向了 BIOS 。

### 硬件自檢 POST (Power On Self Test)

BIOS 程序首先檢查，計算機硬件能否滿足運行的基本條件，這叫做"硬件自檢"（Power On Self Test），縮寫為 [POST](http://en.wikipedia.org/wiki/Power-on_self-test) 。

POST 的主要任務是檢測系統中的一些關鍵設備 (電源、CPU 芯片、BIOS 芯片、定時器芯片、數據收發邏輯電路、DMA 控制 器、中斷控制器以及基本的 640K 內存和內存刷新電路等) 是否存在和能否正常工作。

由於 POST 是最早進行的檢測過程，此時顯卡還沒有初始化，如果系統 BIOS 在進行 POST 的過程中發現了一些致命錯誤，例如沒有找到內存或者內存有問題 （此時只會檢查640K常規內存），那麼系統 BIOS 就會直接控制喇叭發聲來報告錯誤，聲音的長短和次數代表了錯誤的類型。

<!--
接著系統 BIOS 將查找顯示卡的 BIOS ，由顯卡 BIOS 來完成顯示卡的初始化，存放顯卡 BIOS 的 ROM 芯片的起始地址通常設在 C0000H 處，系統 BIOS 在這個地方找到顯卡 BIOS 之後就調用它的初始化代碼，由顯卡 BIOS 來初始化顯卡，此時多數顯卡都會在屏幕上顯示出一些初始化信息，介紹生產廠 商、圖形芯片類型等內容，不過這個畫面幾乎是一閃而過。自此，系統就具備了最基本的運行條件，可以對主板上的其它部分進行診斷和測試。再發現故障時，屏幕上會有提示，但一般不死機。

系統 BIOS 接著會查找其它設備的 BIOS 程序，找到之後同樣要調用這些 BIOS 內部的初始化代碼來初始化相關的設備。 查找完所有其它設備的 BIOS 之後，系統 BIOS 將顯示出它自己的啟動畫面，其中包括有系統 BIOS 的類型、序列號和版本號等內容。

接著，系統 BIOS 將檢測 CPU 的類型和工作頻率，然後開始測試主機所有的內存容量，並同時在屏幕上顯示內存測試的進度，我們可以在 CMOS 設置中自行決定使用簡單耗時少或者詳細耗時多的測試方式。

內存測試通過之後，系統 BIOS 將開始檢測系統中安裝的一些標準硬件設備，這些設備包括：硬盤、CD－ROM、軟驅、串行接口和並行接口等連接的設備，大多數新版本的系統 BIOS 在這一過程中還要自動檢測和設置內存的相關參數、硬盤參數和訪問模式等。

標準設備檢測完畢後，系統 BIOS 內部的“支持即插即用的代碼”將開始檢測和配置系統中已安裝的即插即用設備。每找到一個設備之後，系統 BIOS 都會在屏幕上顯示出設備的名稱和型號等信息，同時為該設備分配中斷、DMA 通道和 I/O 端口等資源。

到這一步為止，所有硬件都已經檢測配置完畢了，多數系統 BIOS 會重新清屏並在屏幕上方顯示出一個表格，其中概略地列出了系統中安裝的各種標準硬件設備，以及它們使用的資源和一些相關工作參數。
-->

接下來系統 BIOS 將更新 ESCD（Extended System Configuration Data，擴展系統配置數據）。ESCD 是系統 BIOS 用來與操作系統交換硬件配置信息的一種手段，這些數據被存放在 CMOS (一小塊特殊的RAM，由主板上的電池來供電) 之中。

<!--
通常 ESCD 數據只在系統硬件配置發生改變後才會更新，所以不是每次啟動機器時我們都能夠看到 “Update ESCD… Success” 這樣的信息，不過，某些主板的系統 BIOS 在保存 ESCD 數據時使用了與 Windows 9x 不相同的數據格式，於是 Windows 9x 在它自己的啟動過程中會把 ESCD 數據修改成自己的格式，但在下一次啟動機器時，即使硬件配置沒有發生改變，系統 BIOS 也會把 ESCD 的數據格式改 回來，如此循環，將會導致在每次啟動機器時，系統 BIOS 都要更新一遍 ESCD ，這就是為什麼有些機器在每次啟動時都會顯示出相關信息的原因。
-->

ESCD 數據更新完畢後，系統 BIOS 的啟動代碼將進行它的最後一項工作，即根據用戶指定的啟動順序從軟盤、硬盤或光驅啟動。

<!--
上面介紹的便是計算機在打開電源開關 (或按 Reset 鍵) 進行冷啟動時所要完成的各種初始化工作，如果我們在 DOS 下按 Ctrl＋Alt＋Del 組合鍵 (或從 Windows 中選擇重新啟動計算機) 來進行熱啟動，那麼 POST 過程將被跳過去，直接從第三步開始，另外第五步的檢測 CPU 和內存測試也不會再進行。
-->

![hardware bios post]({{ site.url }}/images/linux/kernel/hardware_bios_post.png "hardware bios post"){: .pull-center }


### 啟動設備順序

硬件自檢完成後，BIOS 把控制權轉交給下一階段的啟動程序。

這時，BIOS 需要知道，"下一階段的啟動程序" 具體存放在哪一個設備。也就是說，BIOS需要有一個外部儲存設備的排序，排在前面的設備就是優先轉交控制權的設備。這種排序叫做"啟動順序" (Boot Sequence)。

打開 BIOS 的操作界面，裡面有一項就是"設定啟動順序"。

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_bios_bootseq.jpg "hardware bios bootseq"){: .pull-center }

此時， POST 完成了一系列測試和初始化，並將所有的資源整理，包括了中斷、RAM 範圍、I/O 端口等。現在的很多 BIOS 會根據 <a href="http://en.wikipedia.org/wiki/ACPI">Advanced Configuration and Power Interface (ACPI)</a> 保存一系列設備列表，這些列表會被 Kernel 使用。


## 第二階段：主引導記錄

BIOS 按照"啟動順序"，把控制權轉交給排在第一位的儲存設備。這時，計算機讀取該設備的 ["主引導記錄"](http://en.wikipedia.org/wiki/Master_boot_record) (Master boot record，縮寫為 MBR) ，對於硬盤是第一個扇區 (0頭0道1扇區，也就是 Boot Sector)，也即讀取存儲設備最前面的 512 個字節，讀入內存地址 ```0000:7c00~0000:7e00(+200)``` 處。

如果這 512 個字節的最後兩個字節是否為 0x55 和 0xAA (即檢查 (WORD)0000:7dfe 是否等於 0xaa55) ，表明這個設備可以用於啟動，於是跳轉到 0x7C00 並執行；如果不是，表明設備不能用於啟動，控制權於是被轉交給"啟動順序"中的下一個設備。

> 硬盤有很多盤片組成，每個盤片的每個面都有一個讀寫磁頭，如果有 N 個盤片，則有 2N 個面，對應 2N 個磁頭 (Heads) 。每個盤片以電機為圓心，半徑為固定值 R 的同心圓，在邏輯上形成了一個柱面 (Cylinders) 。每個盤片上的每個磁道又分為幾個扇區 (Sector) ，通常每個扇區的容量為 512Bytes 。因此總共 Heads×Cylinders×Sector 個扇區，其編號均從 0 開始。
>
> 在讀取第一個扇區的時候， BIOS 是通過 INT 13 讀取。

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_linux_boot_hard.jpg "hardware bios bootseq"){: .pull-center }

最終的結果是， POST 之後會跳轉到 **0000:7c00** 處執行 MBR 中的程序，將控制權交給 MBR 中的 Bootloader 程序(通常為grub)；以上是由 BIOS 的引導程序完成；下面是由 MBR 中的引導程序完成。

### 主引導記錄

每個操作系統的 Bootloader 通常不一樣，因此，在硬盤中，每個文件系統或者分區都會保留一個引導扇區 (Boot Sector) 來為操作系統安裝 Bootloader 。其格式大致如下。

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_mbr_boot_sector.gif "hardware bios bootseq"){: .pull-center }

> 每個操作系統在安裝時會將 Bootloader 安裝到該分區的 Boot Sector 中去。在 Linux 中可以選擇是否安裝到 MBR 中，如果是，會在 MBR 和分區的 Boot Sector 中都保留一份 Bootloader 。而 Windows 則會主動將 MBR 和 Boot Sector 安裝上 Bootloader 。
>
> Linux 的 Bootloader 在安裝時可以自動搜索其它的 Bootloader ，而 Windows 的卻不可以，因此在安裝雙系統時，最好先安裝 Windows 然後安 Linux 。

主引導扇區 MBR 和操作系統無關，單就功能來說可以將其設置為一個通用的模塊。其主要功能是用來查找活動分區，並將控制權交給相應分區的引導扇區 (第一個扇區) ，也即告訴計算機到硬盤的哪一個位置去找操作系統。

該扇區的是和操作系統有關的，操作系統的引導是由它來完成的，而 MBR 並不負責，MBR 和操作系統無關。引導扇區的任務是把控制權轉交給操作系統的引導程序。

如果使用多系統那麼 MBR 通常也就是 Grub 或這 Lilo 的一部分程序，在安裝時存放在 MBR。Bootloader 中的 MBR 主要操作步驟為：

1. MBR 先將自己的程序代碼由 **0000:7C00h 移動到 0000:0600h** ，然後繼續執行。(注，BIOS 把 MBR 放在 0000:7C00h處)。

2. 掃描分區表，找到一個激活(可引導)分區。在主分區表中搜索標誌為活動的分區，即 80h 標誌(詳見分區表，通常為boot分區)。如果發現沒有活動分區或者不止一個活動分區，則停止，通常為失敗：跳入 ROM BASIC、無效分區表：```goto 5 <1>```。

3. 找到激活分區的起始扇區；讀引導扇區(Boot Sector)，將活動分區的第一個扇區讀入內存地址 0000:7c00 處。失敗：goto 5。注意: 此時讀取的是此扇區的第一個 Sector ，而非 MBR 。

4. 驗證引導扇區最後是否為 55AAh，即檢查(WORD)0000:7dfe 是否等於 0xaa55，若不等於則 ```goto 5 <2/4>``` 。

5. 打印錯誤進入無窮循環，通常的錯誤有：
   1. No active partition.
   1. Invalid partition table.
   1. Error loading operating system.
   1. Missing operating system.

6. 跳轉到 0000:7c00 處繼續執行特定系統的啟動程序，將控制權交給引導扇區代碼，啟動系統。

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_mster_boot_record_0.png "hardware bios bootseq"){: .pull-center }

另一種表示方法，

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_master_boot_record_1.png "hardware bios bootseq"){: .pull-center }

如上圖所示主引導記錄由三個部分組成。

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_marster_records.png "hardware bios bootseq"){: .pull-center }

<!--
<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="2" cellpadding="3" cellspacing="0" width="70%"><tbody>
<tr bgcolor="lightblue"><td align="center">地址</td><td align="center">註釋</td><td align="center">格式</td></tr>
<tr><td align="center">0000-01BD</td><td>Master Boot Record，Bootloader 機器碼，446 個字節</td><td>程序</td></tr>　
<tr><td align="center">01BE-01CD</td><td>分區項1，16 個字節</td><td rowspan="4">分區表(Partition Table)</td></tr>　
<tr><td align="center">01CE-01DD</td><td>分區項2，16 個字節</td></tr>　
<tr><td align="center">01DE-01ED</td><td>分區項3，16 個字節</td></tr>　
<tr><td align="center">01EE-01FD</td><td>分區項4，16 個字節</td></tr>
<tr><td align="center">01FE</td><td>0x55</td><td rowspan="2">結束標誌</td></tr>　
<tr><td align="center">01FF</td><td>0xAA</td></tr>
</tbody></table></center><br>
-->

其中，第二部分"分區表"的作用，是將硬盤分成若干個區。

### 分區表

硬盤分區有很多 [分區](http://en.wikipedia.org/wiki/Disk_partitioning) ，考慮到每個分區可以安裝不同的操作系統，因此 "主引導記錄MBR" 必須知道將控制權轉交給哪個區。

分區表的長度只有64個字節，裡面又分成四項，每項 16 個字節。所以，一個硬盤最多隻能分四個一級分區，又叫做"主分區"。

每個主分區的 16 個字節，由 6 個部分組成：

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_marster_records_details.png "hardware bios bootseq"){: .pull-center }

<!--
<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="2" cellpadding="3" cellspacing="0" width="70%"><tbody>
<tr bgcolor="lightblue"><td align="center" width="75">地址</td><td align="center">註釋</td></tr>
<tr><td align="center">0</td><td>0x80 - 活動分區，0x00 - 非活動分區；控制權會交給活動分區。四個主分區裡面有且只只能有一個是活動分區。</td></tr>　
<tr><td align="center">1-3</td><td>主分區第一個扇區的物理位置，磁頭(第1個字節)、扇區號(第2字節低6位)、柱面(第2字節高2位+第3字節)。</td></tr>　
<tr><td align="center">4</td><td><a href="http://en.wikipedia.org/wiki/Partition_type"> 主分區類型 </a>：
<ul><li>
    00h 該分區未使用。</li><li>
    05h 擴展分區。</li><li>
    06h FAT16 基本分區。</li><li>
    07h NTFS 分區。</li><li>
    0Bh FAT32 基本分區。</li><li>
    0Fh 擴展分區。
</li></ul></td></tr>　
<tr><td align="center">5-7</td><td>主分區最後一個扇區的物理位置，磁頭(第1個字節)、扇區號(第2字節低6位)、柱面(第2字節+第3字節)。</td></tr>　
<tr><td align="center">8-11</td><td>該分區起始扇區號。</td></tr>
<tr><td align="center">12-15</td><td>該分區的扇區總數。</td></tr>　
</tbody></table></center><br>
-->

最後的四個字節 ("主分區的扇區總數")，決定了這個主分區的長度。也就是說，一個主分區的扇區總數最多不超過 2 的 32 次方。

如果每個扇區為 512 個字節，就意味著單個分區最大不超過 2TB 。再考慮到扇區的邏輯地址也是 32 位，所以單個硬盤可利用的空間最大也不超過 2TB 。如果想使用更大的硬盤，只有 2 個方法：一是提高每個扇區的字節數，二是增加 [扇區總數](http://en.wikipedia.org/wiki/GUID_Partition_Table) 。

> 硬盤的分區規則：一個分區的所有扇區必須連續，硬盤可以有最多 4 個物理上的分區，這 4 個物理分區可以時個主分區或者 3 個主分區加 1 個擴展分區，只能由一個擴展分區。在擴展分區裡，可以而且必須再繼續劃分邏輯分區(邏輯盤)，可以有任意多個。
>
> 擴展分區的信息位於硬盤分區表(DPT)中，而邏輯驅動器的信息則位於擴展分區的起始扇區，即該分區的起始地址(面/扇區/磁道)所對應的扇區，該扇區中的信息與硬盤主引導扇區的區別是不包含 MBR ，而 16 字節的分區信息則表示的是邏輯驅動器的起始和結束地址等。

對於主引導扇區最多可以識別 4 個主分區，為了可以使用更多的分區，引入了擴展分區。擴展分區中的邏輯驅動器引導記錄採用鏈式結構。每一個邏輯分區都有一個和 MBR 分區表類似的擴展引導記錄 EBR 。該分區表的第一項指向該邏輯分區本身的引導扇區，第二項指向下一個邏輯驅動器的 EBR ，其格式如下所示。

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_ebr_1.gif "hardware bios bootseq"){: .pull-center } ![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_ebr_2.gif "hardware bios bootseq"){: .pull-center }

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_ebr_3.png "hardware bios bootseq"){: .pull-center }

<!--
<center><a href="http://en.wikipedia.org/wiki/Extended_boot_record">擴展引導記錄</a><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="2" cellpadding="3" cellspacing="0" width="70%"><tbody>
<tr bgcolor="lightblue"><td align="center">地址</td><td align="center">註釋</td><td align="center">格式</td></tr>
	<tr><td align="center">0000-01BD</td><td>通常沒有使用，一般為 0 。</td><td>空閒</td></tr>　
	<tr><td align="center">01BE-01CD</td><td>分區項1，16 個字節。本擴展分區。</td><td rowspan="4">分區表(Partition Table)</td></tr>　
	<tr><td align="center">01CE-01DD</td><td>分區項2，16 個字節。下一個擴展分區。</td></tr>　
	<tr><td align="center">01DE-01ED</td><td>分區項3，16 個字節。未使用。</td></tr>　
	<tr><td align="center">01EE-01FD</td><td>分區項4，16 個字節。未使用。</td></tr>
	<tr><td align="center">01FE</td><td>0x55</td><td rowspan="2">結束標誌</td></tr>　
	<tr><td align="center">01FF</td><td>0xAA</td></tr>
</tbody></table></center>
-->

### MBR 操作

在 Linux 中可以通過如下的命令對 MBR 進行操作，包括備份、刪除等。

{% highlight text %}
# fdisk -l                                   // 查看硬盤，假設為 /dev/sda
# dd if=/dev/sda of=mbr.bak bs=512 count=1   // 備份 MBR
# dd if=mbr.bak of=/dev/sda bs=512 count=1   // 恢復 MBR
# dd if=/dev/zero of=/dev/sda bs=446 count=1 // 刪除 MBR

# vim mbr.bak -b                             // 以二進制打開
:%!xxd                                       // 切換為十六進制顯示
{% endhighlight %}

## 第三階段：硬盤啟動

這時，計算機的控制權就要轉交給硬盤的某個分區了，這裡又分成三種情況。

### 情況A：卷引導記錄

上一節提到，四個主分區裡面，只有一個是激活的。計算機會讀取激活分區的第一個扇區，叫做 [卷引導記錄](http://en.wikipedia.org/wiki/Volume_Boot_Record) (Volume boot record，縮寫為VBR) 。

"卷引導記錄"的主要作用是，告訴計算機，操作系統在這個分區裡的位置。然後，計算機就會加載操作系統了。

### 情況B：擴展分區和邏輯分區

隨著硬盤越來越大，四個主分區已經不夠了，需要更多的分區。但是，分區表只有四項，因此規定有且僅有一個區可以被定義成"擴展分區" (Extended partition)。

所謂"擴展分區"，就是指這個區裡面又分成多個區。這種分區裡面的分區，就叫做"邏輯分區" (logical partition)。

計算機先讀取擴展分區的第一個扇區，叫做 [擴展引導記錄](http://en.wikipedia.org/wiki/Extended_partition) （Extended boot record，縮寫為EBR）。它裡面同樣也包含一張 64 字節的分區表，但是隻有前兩項是有效的，分別指向當前分區的磁盤地址，以及下一個邏輯分區的磁盤地址，從而形成了一個有序的鏈表。直到某個邏輯分區的分區表只包含它自身為止 (即只有一個分區項)。因此，擴展分區可以包含無數個邏輯分區。<br><br>

但是，似乎很少通過這種方式啟動操作系統。如果操作系統確實安裝在擴展分區，一般採用下一種方式啟動。

### 情況C：啟動管理器

在這種情況下，計算機讀取"主引導記錄"前面 446 字節的機器碼之後，不再把控制權轉交給某一個分區，而是運行事先安裝的 [啟動管理器](http://en.wikipedia.org/wiki/Boot_loader) (boot loader)，由用戶選擇啟動哪一個操作系統。

Linux環境中，目前最流行的啟動管理器是 [Grub](http://en.wikipedia.org/wiki/GNU_GRUB) 。

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_boot_grub.png "hardware bios bootseq"){: .pull-center }

### X86 Bootloader 設計

Linux 運行在保護模式下，但是當機器啟動復位的時候卻處於實模式下，所以 Bootloader 做的工作也是在實模式之下的，使用低端的 640k 內存。

早期的磁盤，每個磁道有 63 個扇區，硬盤在使用中，0 面的 0 磁道的所有扇區即前 63 個扇區做為特殊用途，BIOS 和操作系統不放置任何數據於此，一般做為 bootloader 使用，這也是為什麼 GRUB 必須使用 stage1.5 的原因（因為其大小一般為 100KB 左右，超出了 62 個扇區的限制）。

<!--
grub 引導的 linux 的過程不太清楚，有待添加。？？？？？？？？？？？？？？？？
2)       BIOS -> MBR(GRUB2 boot.img)
此場景中MBR引導代碼即GRUB2的boot/i386/pc/boot.S編譯出來的代碼，稱為boot.img；代碼執行位置即0x7C00，主要行為：設置堆棧指針為0x2000處，探測硬盤CHS/LBA工作模式，並加載第二個扇區最終至0x8000處，並跳至0x8000執行；
-->

### Grub Stage2

當 Grub-Stage2 被載入內存執行時，它首先會去解析 grub 的配置文件 ```/boot/grub/grub.conf``` ，然後加載內核鏡像到內存中，並將控制權轉交給內核。控制權轉交給操作系統後，操作系統的內核首先被載入內存。

bootloader 執行完成後內存的分佈如下圖所示，詳見 [Documentation/i386/boot.txt]({{ site.kernel_docs_url }}/Documentation/i386/boot.txt) Linux-x86 的啟動協議。

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_memory_after_bootloader.png "hardware bios bootseq"){: .pull-center }

現在 Linux 的鏡像已經加載到了內存中，主要包括了兩部分：A) 運行在實模式中，加載到了 640K 以下的內存；B) 運行在保護模式的內核，加載到了 1M+ 。

<!--
通常從系統上電到運行到linux kenel這部分的任務是由boot loader來完成。Boot loader在跳轉到kernel之前要完成一些限制條件:
1、CPU必須處於SVC(supervisor)模式,並且IRQ和FIQ中斷必須是禁止的；
2、MMU(內存管理單元)必須是關閉的, 此時虛擬地址對應物理地址；
3、數據cache(Data cache)必須是關閉的；
4、指令cache(Instruction cache)沒有強制要求；
5、CPU通用寄存器0(r0)必須是0;
6、CPU通用寄存器1(r1)必須是ARM Linux machine type；
7、CPU通用寄存器2(r2)必須是kernel parameter list的物理地址；
-->

上圖所示的 real-mode kernel header 之上的內容在 Linux boot protocol 中定義，主要用來 Bootloader 和 Linux Kernel 之間進行通訊。其中包括了 Bootloader 運行時寫入的內容，如果內核版本、內核參數的地址等。當完成了一系列初始化之後，那麼就會跳轉到 Linux Kernel 中執行。

首先加載的是 setup.bin，其中包括了開始的 512(0x200)bytes 的 bootsector 用於兼容之前的版本，正常應該不會執行的，如果執行會輸出錯誤信息。可以通過 vim+%!xxd 打開 setup.bin 文件查看，0x200 處為 eb 也就是一個調轉指令，也就是入口 _start。

其實在 GRUB 等 bootloader 將內核的 setup.bin 讀到 0x90000 處，將 vmlinux 讀到 0x100000 處(grub拷貝時會打開保護模式,然後關閉)，然後跳轉到 0x90200 開始執行，恰好跳過了 512 字節的 bootsector。

那麼接下來就將 CPU 的執行權限交給了 Linux 。




## 參考

介紹開關鍵按下到 BIOS 啟動，含有硬件的介紹，可以參考 [CPU Reset](/reference/linux/kernel/CPU_Reset.pdf)；以及 [Big Real Mode 的介紹](/reference/linux/kernel/Introduction_to_Big_Real_Mode.pdf) 。

[All the Details of many versions of both MBR and OS Boot Records](http://thestarman.pcministry.com/asm/mbr/index.html) 對各個版本 MBR 的介紹，含有 Windows 下的工具，十分詳細。

<!--
<a href="http://www.ibm.com/developerworks/cn/linux/embed/l-bootloader/"> Linux bootloader 編寫方法 [Web] </a> IBM 介紹如何編寫 Bootloader </li><li>
<a href="reference/lfs/linux_x86_boot_protocol.org.txt"> THE LINUX/x86 BOOT PROTOCOL [local] </a> 原版，摘自 Linux-3.4.48 源碼 Documentation/x86/boot.txt </li><li>
<a href="reference/lfs/linux_x86_boot_protocol.txt"> THE LINUX/x86 BOOT PROTOCOL [local]</a> 中文翻譯版</li><li>
<a href="http://blog.csdn.net/yunsongice/article/category/759408">系統初始化 [Web]</a> 內核編譯、初始化過程，以及其它的一些操作，介紹比較詳細
-->


{% highlight text %}
{% endhighlight %}
