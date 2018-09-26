# C++ 影像串流 Raspberry Pi camera RTSP 影像串流


http://jex.logdown.com/posts/176992-raspberry-pi-camera-rtsp-video-streaming

RASPBERRY PI CAMERA RTSP 影像串流

##什麼是 RTP/RTCP/RTSP/RTMP
- RTP (Real-time Transport Protocol) 是一種傳遞音訊和視訊的協定, 主要應用在 UDP 上, 大多用在`一對一傳播`

- RTCP (Real-time Transport Control Protocol) 為 RTP 提供 out-of-band 控制, `本身不傳輸數據, 但和RTP一起協作將 media data 打包和發送`

- RTSP (Real Time Streaming Protocol) `控制聲音及影像的多媒體串流協定`, 運作跟 HTTP/1.1 類似, 支援 `Multicast`, 不強調時間同步等特性, `通常處理 RTP 及 RTCP 協定使用, 可選擇 TCP 或 UDP 傳送
RTMP(Real Time Messaging Protocol)` 是`Adobe Flash 的多媒體串流協定`




##什麼是 LIVE555
是一套 C++ 的函式庫, 可以實作出 RTSP/RTP server, VLC 是基於此函式庫開發的
支援傳輸方式
- RTP over UDP
- RTP over RTSP
- RTP/RTSP over HTTP

等等..
支援的影音格式

- H.264
- MPEG4
- MP3

等等..

### INSTALL LIVE555
Easiest way is first ensure you have the appropriate compiler:

```sh
Easiest way is first ensure you have the appropriate compiler:
sudo apt-get install build-essential


then make sure you don't have the repository live555 libraries on your system:
sudo apt-get remove liblivemedia-dev
```

```sh
and finally download, build and install the libraries:

cd /tmp
wget http://www.live555.com/liveMedia/public/live555-latest.tar.gz
tar xvf live555-latest.tar.gz
cd live
./genMakefiles linux
make
sudo cp -r /tmp/live /usr/lib
make clean
```

## 啟動 RTSP SERVER

將影音檔 (h.264, mp4 etc..) copy 到 /usr/lib/live/mediaServer 下
h.264 副檔名為 .264, mp4 副檔名為 .m4e

###執行

```sh
cd /usr/lib/live/mediaServer
./live555MediaServer
```

安裝並打開 VLC 播放器, 選擇播放網路串流,
輸入 : rtsp://54.250.138.78:8554/akb.m4e
建議在 /usr/lib/live/mediaServer 建立 my_video 資料夾, 將影音與程式分開 

```sh
(rtsp://54.250.138.78:8554/my_video/akb.m4e)
```

### 播放
```sh
ffplay -i rtsp://175.181.179.39:8554/wwe.m4e 
```

用 RASPBERRY PI CAMERA 播放即時影像
啟動 RTSP server 及接收 Raspberry Pi 傳來的資料儲存成檔案:

```sh
cd /usr/lib/live/mediaServer/my_video
touch rpi.264
nc -l 8080 | pv -b > rpi.264
```


將 camera 的影像傳到遠端 server :

```sh
raspivid -t 999999 -h 180 -w 270 -o - | pv -b | nc 54.250.138.76 8080
```


```sh
VLC -> open network : rtsp://54.250.138.78:8554/my_video/rpi.264
```

##後記

想要做一個能讓很多人同時看即時影像的 server 另一方面是考量到 Raspberry Pi 的效能,
所以才把 camera 跟 RTSP 分開, 但效果不太好, 傳輸速率很慢..即使不用 wifi 傳輸也一樣很慢,
大概過 10 幾秒才能看到影像 (這已經是將影像的畫質調很低的狀態下),
不知道是不是我的 ec2 免費方案不夠力的關係呢?
還是有更好的方法可以改進?
對於我這種三角貓等級的工程師來說只能打打指令過過乾癮,
從頭到尾沒寫半行 code 做出來的 偽 即時影像還蠻心虛的,
因為我還是不瞭解底層到底做了哪些事..
而且用現成的 RTSP server 不就沒辦法針對影片進行權限區分了嗎?
現在腦中還是充滿很多問號..

###Q & A

發生錯誤 : `StreamParser internal error (149997 + 4 > 150000) Aborted (core dumped)`
原因 : 傳送一幀非常大的 H.264 影像 (default 150000) 造成的
查到的解決方法 :

```sh
修改 /usr/lib/live/liveMedia/StreamParser.cpp,
```

找到 BANK_SIZE 建議將值設為 150000~300000 之間
改完要 compile :

```sh
cd /usr/lib/live
sudo make
```

但我仍然還是一樣發生此 buffer 問題, 但播放 mp3 及 aac 沒問題
此情況似乎是發生在 H.264 parsing 才有的問題, 或是 mp4 都有這問題, 試了很多 mp4 影片, 都還是一樣發生
但這邊下載的 .ts 影片是能播放的


隨便說說..
現在多數瀏覽器支援播放 H.264 影片但可惜沒有瀏覽器的 HTML5 播放器支援播放 RTSP 串流影片, 所以只能透過像 VLC 這種播放器來播放,
目前 RTMP 應該算是 live streaming 的通用解, 看來要擺脫 flash 還有很長一段路要走
比較可惜的還有 webm (vp8) 無法普及, 不然它的編解碼效率也很不錯, 重要的是它是開源的
其他
Realplayer : 原生不支援 H.264 影片, 必須安裝 quicktime plugin 才能對 H.264 解碼
Quicktime player : 不支援 RTP/AVP/TCP 傳輸, RTP/AVP (UDP) 傳輸不包括 NAT 打洞, 因此唯一適合的傳輸是 HTTP tunneling
VLC player : RTP/AVP 傳輸也不支援 NAT 打洞, 但可以用 RTP/AVP/TCP 傳輸

----------------
http://www.programmer-club.com.tw/ShowSameTitleN/vc/39380.html

- RTSP ( 如果已經瞭解 http 的話, 這個應該不難. RFC2326 )
- RTCP
- RTP
- SDP
- 
- MPEG2-TS
- 
- Unicast
- Multicast
- 
- IGMP ( for IPv4 )
- MLD ( for IPv6 )
- 
- Demux

至於 encoder , decoder 就看您是否需要深入瞭解了.

推薦幾個常用的 open source 的 library:
- live555

live555 包括 streaming server ( live555mediaserver ), streaming client ( openRTSP ).
    這個 library 主要是提供網路層的資料傳輸. client 有沒有 player 我就不清楚了.

- VLC

vlc 底層是以 live555 實作的, 並在同一支程式提供了 streaming server,
    streaming client, 也將 decoder 整合進 player, 可以直接播放, 看到畫面.
- GStreamer

media streaming framework 一個很多 embedded system 都在使用的 framework.

這幾個也都是跨平臺的 library, live555 目前好像只能當 rtsp server.
VLC 可以用來當各種 server, 只是似乎沒有 live555 穩定. 但他的 multicast server 很方便使用,
可以將很多 multicast stream 都描述在一個檔案內, 一次全部打出來.
使用 Multicast 時, 如果在 local 環境測試, 請小心把網路頻寬佔滿.
