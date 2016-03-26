# Application 效能分析有妙招 — 使用 perf 走天下


在手機上面，Application 的實作往往會影響到效能好壞以及是否夠省電，其中 Application 的 CPU 使用量會是一個非常關鍵的因素。

以下就拿使用 Firefox OS 的手機來做舉例說明，首先透過下面的 top 指令，可以看到整體 System 以及 Application 的 CPU 使用量，有助於了解 System 或者是 Application 的負載。

```sh
peter@pchang:~$ adb shell top -s cpu -m 10 -d 1
```
```sh
User 77%, System 18%, IOW 2%, IRQ 0%
User 65 + Nice 30 + Sys 23 + Idle 2 + IOW 3 + IRQ 0 + SIRQ 0 = 123

  PID PR CPU% S  #THR     VSS     RSS PCY UID      Name
  108  0  59% R    44 180304K  60156K  fg root     /system/b2g/b2g
  483  0  26% R    11  85824K  34104K  fg app_483  /system/b2g/plugin-container
  563  0   4% R     1   1056K    408K  fg root     top

```

```sh
peter@pchang:~$ adb shell top -s cpu -m 10 -d 1
```

```
User 77%, System 18%, IOW 2%, IRQ 0%
User 65 + Nice 30 + Sys 23 + Idle 2 + IOW 3 + IRQ 0 + SIRQ 0 = 123
 
  PID PR CPU% S  #THR     VSS     RSS PCY UID      Name
  108  0  59% R    44 180304K  60156K  fg root     /system/b2g/b2g
  483  0  26% R    11  85824K  34104K  fg app_483  /system/b2g/plugin-container
  563  0   4% R     1   1056K    408K  fg root     top

```


為了要進一步分析 Application 的效能瓶頸，可以使用 Linux 上分析的工具， `perf` (Download from Tegra Android Toolkit)。
Perf 是根據 Linux Kernel 已經定義的 perf_event 來紀錄程式運作的過程，所以只要是 Linux based system 都可以支援，包含 Android/Firefox OS phone。目前 perf_event 在 Linux Kernel 上預設為啓動，所以使用 perf 只要準備好 Application debug symbols 就可以來進行分析。有興趣的朋友可以參考 perf kernel support ，來進一步確定你的手機是否有啟動 perf_event。

Perf 上也有類似 top 指令，可以看到整體 System 的負載，譬如下面的結果，就可以看到 WebGLContext::Conver 佔用了最多 CPU 資源。

```sh
peter@pchang:~$ adb shell perf top
```

```sh
-------------------------------------------------------------------------------
   PerfTop:     325 irqs/sec  kernel:24.9%  exact:  0.0% [1000Hz cycles],  (all, 1 CPUs)
-------------------------------------------------------------------------------
 
         samples  pcnt function                      DSO
         _______ _____ _____________________________ ______________________
 
          697.00 50.6% mozilla::WebGLContext::Conver /system/b2g/libxul.so 
          424.00 30.8% yamato_tile_texture           libGLESv2_adreno200.so
          124.00  9.0% yamato_untile_texture         libGLESv2_adreno200.so
           26.00  1.9% rb_surface_read               libGLESv2_adreno200.so
           11.00  0.8% mozilla::gl::SwapRAndBCompone /system/b2g/libxul.so 
            5.00  0.4% yamato_texfmt_to_pixfmt_for_m libGLESv2_adreno200.so
```

也可以透過 perf record/perf report ，來針對特定的 Application 做負載的分析。不過在分析之前，需要準備除錯用的 gecko modules 和對應的 debug symbols。以 Firefox OS 為範例，先做下面的修改，然後重新編譯和取代 gecko modules 到手機上，這樣是為了把 debug symbols 放到編譯好的 gecko modules，之後才能得到詳盡的分析結果。

```sh
peter@pchang:~/B2G/gonk-misc$ git diff ./default-gecko-config
diff --git a/default-gecko-config b/default-gecko-config
index c14919a..9a18176 100644
--- a/default-gecko-config
+++ b/default-gecko-config
@@ -25,6 +25,7 @@ ac_add_options --enable-debug-symbols
 if [ "${B2G_DEBUG:-0}" != "0" ]; then
 ac_add_options --enable-debug
 fi
+ac_add_options --enable-profiling
 
 if [ "${B2G_NOOPT:-0}" != "0" ]; then
```

透過下面的指令把 debug symbols 存放在 tmp folder 下面。

```sh
cp -pr out/target/product/device_name/system/ /tmp/b2g_perf/system/
cp -pr out/target/product/device_name/symbols/system/ /tmp/b2g_perf/system/
cp -pr objdir-gecko/dist/bin/b2g /tmp/b2g_perf/system/b2g/
cp -pr objdir-gecko/dist/lib/*.so /tmp/b2g_perf/system/b2g/
```

接下來可以開始分析 Application 運作上的瓶頸。
Perf record 會針對 Application 記錄 20 秒的運作訊息，接著把記錄的資料從手機上下載出來。

最後使用 perf report 就可以看到使用 CPU 資源最多的函式名稱，接下來開發者就可以開始進行評估是否有改善的計劃讓 Application 運行的更快更省電。


```sh
peter@pchang:~$ adb shell perf record -p pid -o /data/local/perf.data sleep 20
peter@pchang:~$ adb pull /data/local/perf.data /tmp/b2g_perf/
 
peter@pchang:~$ perf report --symfs=/tmp/b2g_perf/system/ --sort dso,symbol
 
 71.38%  libxul.so                 [.] mozilla::WebGLContext::Conver
 15.28%  [ath6kl_sdio]             [k] 0x106fd7c
  7.40%  libGLESv2_adreno200.so    [.] yamato_tile_texture
  1.81%  libc.so                   [.] memcpy
```

Perf 還有更多的功能，有興趣的讀者可以參考下面的連結。
https://perf.wiki.kernel.org/index.php/Tutorial

 
