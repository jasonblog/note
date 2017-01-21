# u-boot 組態研究- raspberry pi 2


這份文件主要研究當uboot的組態設定成raspberry pi 2時，他的config file開了什麼compile options.

實際上可以用make menuconfig去看。但是記得要先裝ncurse套件。「這個套件主要在命令列底下提供滑鼠支援，建立視窗（UI），處理顏色等等http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/intro.html」


```sh
$ sudo apt-get install libncurses5-dev
```

![](./images/hugh_driver.hackpad.com_bpfo4wmPuGP_p.493406_1453295416695_Screenshot from 2016-01-20 21-09-28.png)

