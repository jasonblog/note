# [Raspberry Pi Project] kernel啟動參數


在之前的文章有說明到RPi啟動時,是將kernel啟動參數放在boot區內的cmdline.txt,在這篇文章將裡面每個參數做一下說明

- dwc_otg.lpm_enable=0 

dwc_otg指的是HS OTG USB Controller driver相關參數.例如lpm_enable=0 即預設USB HS OTG device的Link Power management為關閉.(如果修改系統把把rootfs搬到USB硬碟上,這個值可能就要改成1了吧?)


- console=ttyAMA0,115200

RPi用ttyAMA0做為UART port(GPIO 8 and 10)的終端定義,所以這裡表示在UART port上開console.如果後續需要使用UART為其他用途,則需要把這部分拿掉,以免干擾


- kgdboc=ttyAMA0,115200 


在UART port上開KGDB debug port. 如果後續需要使用UART為其他用途,則需要把這部分拿掉,以免干擾

- console=tty1

在tty1上開console

- root=/dev/mmcblk0p2 

定義rootfs path. 如果你希望把rootfs改放到USB disk或者NFS上,這裡可以修改 

- rootfstype=ext4 

rootfs格式

- elevator=deadline 

kernel預設的I/O scheduling的方法(例如noop, deadline , .....)這個比較明顯的影響應該是對於storage IO的效能　 

- rootwait

kernel要先初始化MMC storage module(可能要花點時間)後才能完成mount MMC,而預設的rootfs又是放在MMC上,所以利用此參數要求kernel在載入全部的module後再開始mount and read rootfs