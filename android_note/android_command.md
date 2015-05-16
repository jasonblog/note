# Android指令和命令參數


主要用這篇是因為方便我以後查詢Android相關指令，感謝來原作者整理出這些!

### Android模擬器命令列啟動模式
在android-sdk-windows-1.1\tools執行emulator以執行模擬器
加上-skin參數，指定顯示模式為HVGA-L，則可轉為橫向
```
emulator - skin HVGA-L (480*320，水平顯示)
emulator - skin HVGA-L (320*480，垂直顯示，模擬器預設模式)
emulator - skin HVGA-L (320*240，水平顯示)
emulator - skin HVGA-L (240*320，垂直顯示)
```

### 使用mksdcard指令模擬1GB的記憶卡
```
mksdcard 1024M sacard.img
```
### 模擬插入 SD 卡的模擬器
```
emulator - sdcard sdcard.img
```

#### 使用 adb+push 上載檔案到SD記憶卡
```
adb push 001.jpg /sdcard (複製檔案到 /sdcard 目錄下)
adb push pictures /sdcard (複製 picture 照片目錄到 /sdcard 目錄下)
adb push mp3 /sdcard (複製 mp3 音樂目錄到 /sdcard 目錄下)
adb shell (Android 模擬器啟動命令列模式)
#cd /sdcard (進入 /sdcard 目錄)
#ls (查看 SD 記憶卡中的檔案)
```
### 使用 adb+pull 從 SD 記憶卡下載檔案
```
adb pull /sdcard/001.jpg . (下載 /sdcard 目錄下的檔案)
adb pull /sdcard/pictures . (下載 sdcard 目錄下的 pictures 目錄)
```

### 刪除 SD 卡裡面的檔案
```
adb shell
#ced /sdcard
#rm 001.jpg (刪除 SD 記憶卡裡的檔案)
#rm -r * (刪除 SD 記憶卡裡所有檔案與目錄)
```

### Android模擬器影片播放方法
```
mksdcard 4096M video.img (製作一個影像檔的 SD 記憶卡)
adb push video.avi /sdcard (從電腦複製影像檔到 SD 卡中)
emulator -sdcard video.img (啟動模擬器並載入 SD 卡)
下載免費的影片播放軟體，ex: Meridian Video Player (iiivpa.apk)
http://sites.google.com/site/eternalsandbox/Home/meridian-video-player
adb install iiivpa.apk (安裝Meridian Video Player)
接下來就可以用裝上去的player播放.mp4、3gp與.wmv三種檔案格式
```

### 安裝 APK 應用程式
```
adb install filename.apk (安裝filename.apk)
adb install -r filename.apk (保留已設定資料，重新安裝filename.apk)
adb -s emulator-5554 install filename.apk (指定安裝 APK 套件在 5554 的 Android 模擬器中)
```

### 移除 APK 應用程式
```
adb uninstall package
adb uninstall -k package (移除程式時，保留資料)
此package名稱不是安裝APK套裝時的檔名或顯示在模擬器中的應用程式名稱
可以先到/data/data或data/app目錄下，查詢想移除的package名稱
adb shell
ls /data/data 或 /data/app (查詢 Package 名稱)
exit
adb uninstall package (移除查詢到的 Package)
```

### ADB 系統除錯與連結工具
```
$adb devices (顯示目前有多少個模擬器正在執行)
$adb -s <serialNumber> <command> (指定模擬器來操作)
adb -s emulator-5554 install email.apk
$adb install apkfile (安裝 APK 應用程式套件)
adb install email.apk
$adb uninstall package (移除 APK 應用程式套件)
adb uninstall com.android.email
$adb shell (進入 Android 系統指令列模式)
$ls
$dmesg (查看 Android Linux Kernel 運作訊息)
ls - 顯示檔案目錄
cd - 進入目錄
rm - 刪除檔案
mv - 移動檔案
mkdir - 產生目錄
rmdir - 刪除目錄

$adb push <file/dir> (複製檔案到 SD 卡)
adb push mp3 /sdcard
$adb pull <file/dir> . (從 Android 系統下載檔案)
adb pull /data/app/com.android.email
$adb logcat (監控模擬器運作紀錄，以Ctrl + c 離開監控模式)
$adb bugreport (產生 adb 除錯報告)
$adb get-state (獲得 adb 伺服器運作狀態)
$adb start-server (啟動 adb 伺服器)
$adb kill-server (關掉 adb 伺服器)
$adb forward tcp:6100 tcp:7100 (更改模擬器網路 TCP 通訊埠)
$adb shell ps -x (顯示 Android 上所有正在執行的行程)
$adb version (顯示 adb 版本)
$adb help (顯示 adb 指令參數)
```

### Emulator 命令列啟動參數
```
emulator -timezone Asia/Taipei (指定時區)
emulator -no-boo-anim (省略開機小機器人動畫畫面)
emulator -scale auto (調整模擬器視窗大小)
emulator - scale factor (factor: 0.1-3.0)
emulator -dpi-device 300 (更改模擬器的解析度，default為 165dpi)
emulator -skin <skinID> (更改模擬器顯示模式)
emulator -help-keys (顯示鍵盤快速鍵說明)
emulator -shell (相當於adb shell 功能)
emulator -data data.img (使 /data 目錄使用 data.img 的檔案空間)
emulator -sdcard sdcard.img (使 /sdcard 目錄使用 sdcard.img 的檔案空間)
emulator -cache cache.img (瀏覽器暫存檔儲存空間)
emulator -wipe-data (使模擬器恢復到原廠設定)
emulator -help (顯示 emulator 指令參數)
```
