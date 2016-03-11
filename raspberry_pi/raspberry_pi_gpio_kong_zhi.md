# Raspberry pi GPIO 控制


大概的流程就是(N 為希望設定的GPIO 號碼(跟物理位置號碼不同喔))：

1. 設定：
echo "N" > /sys/class/gpio/export

2. 設定輸入或輸出(需接在上一步驟之後)：
echo "in" > /sys/class/gpio/gpioN/direction
echo "out" > /sys/class/gpio/gpioN/direction

3. 讀取或輸出
讀取：cat /sys/class/gpio/gpioN/value
輸出：echo "1" > /sys/class/gpio/gpioN/value

4. 結束並取消設定 
echo "N" > /sys/class/gpio/unexport

如果想寫程式，可以考慮用pigpio函式庫，它支援所有GPIO的功能：
http://abyz.co.uk/rpi/pigpio/index.html
其中的piscope更可以監看各個pin的狀態，相當方便。

或是用BCM2835 C Library
http://www.airspayce.com/mikem/bcm2835/

可是最快的還是用C已經有的函式庫
http://codeandlife.com/2012/07/03/benchmarking-raspberry-pi-gpio-speed/

##參考資料：
elinux.org/RPi_Low-level_peripherals#sysfs 