# [Raspberry Pi Project] How does Raspberry Pi boot?


一般嵌入式系統都是使用flash來當作system/storage Partition使用,但是RPi上卻是採用SD卡當system/storage partition,因為我過去的經驗中並沒有這樣的project經驗,所以讓我想研究一下RPi的系統啟動流程(以下的內容是以raspbian 3.6.11例子)

## SD card partition

我們裝好的raspbian SD卡會被分成兩塊partition: FAT32的boot partition, EXT4的linux root partition. 對開機流程中,最重要關心的就是boot partition內的幾個檔案:
- bootcode.bin : 2nd stage bootloader
- start.elf / start_cd.elf / start_x.elf: The GPU binary firmware image (start_cd.elf是針對256MB RAM的model使用; start_x.elf是功能測試版)
- fixup.dat / fixup_cd.dat / fixup_x.dat: is used to configure the SDRAM partition between the GPU and the CPU (fixup_cd.dat是針對256MB RAM的model使用; fixup_x.dat是功能測試版)
- kernel.img: The OS kernel to load on the ARM processor.  
- kernel_emergency.img : kernel with busybox rootfs. You can use this to repair the main linux partition using e2fsck if the linux partition gets corrupted. 
- cmdline.txt: Parameters passed to the kernel on boot. 
- config.txt: A configuration file read by the GPU. Use this to override set the video mode, alter system clock speeds, voltages, etc. 
上面提到的bootcode.bin , start.elf, fixup.dat 因為都不是open source(應該都是Broadcom 維護),所以只能直接抓binary更新(git://github.com/raspberrypi/firmware.git)


上面提到的bootcode.bin , start.elf, fixup.dat 因為都不是open source(應該都是Broadcom 維護),所以只能直接抓binary更新(git://github.com/raspberrypi/firmware.git)

而kernel.img的部分就是我們可以自己抓source code下來re-build的部分(另一篇文章會提到)

## Boot Process

- 當RPi一開機時,ARM core是off的狀態,GPU會先切到on. 此時SDRAM還是disabled.
- BCM2835上有一塊ROM燒好了first stage bootloader(出廠就燒好了,使用者不能再更新了).因此GPU一開始就會執行first stage bootloader. First stage bootloader會負責mount SD卡上的boot partition, 然後載入boot partition上的 bootcode.bin (the second stage bootloader) 到L2 cache上, 最後把執行權轉給bootcode.bin .
- bootcode.bin 會負責enables SDRAM,接著讀入 start.elf(GPU firmware).接著把執行權轉給start.elf
- start.elf負責喚起ARM core,並且依照fixup.dat內的設定載入kernel.img(linux kernel),接著讀取config.txt, cmdline.txt,依設定執行kernel(此時應該已經是ARM core執行了)
- Linux kernel啟動部分就不在此研究了

##[參考]
http://elinux.org/RPi_Advanced_Setup#Setting_up_the_boot_partition

http://elinux.org/RPi_Software
