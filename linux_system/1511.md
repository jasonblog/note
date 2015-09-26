# Linux 的 time 指令

Linux 有個很有意思的 time 指令，可以用來查看另一個指令的執行時間，例如執行 time helloworld 會顯示 helloworld 這支程式的執行時間。

time 指令的返回值包含「實際時間 (real time)」、「用戶態 CPU 時間 (user CPU time)」及「核心態 CPU 時間 (system CPU time)」。其中，

### real time 表示後面所接的指令或程式從開始執行到結束終止所需要的時間。簡單講，當一個程式開始執行瞬間看一下手錶記下時間，當程式結束終止瞬間再看一次手錶，兩次的時間差就是 real time。
### user CPU time 表示程式在 user mode 所佔用的 CPU 時間總和。多核心的 CPU 或多顆 CPU 計算時，則必須將每一個核心或每一顆 CPU 的時間加總起來。
### system CPU time 表示程式在 kernel mode 所佔用的 CPU 時間總和。
所以，以下的想法就不一定成立：

##一：real time = user CPU time + system CPU time

這裡舉一個比較極端的例子，假設我執行 time sleep 10 這個命令，time 會返回給我的訊息如下：
```sh
real    0m10.003s
user    0m0.004s
sys     0m0.000s
```

因為動作是「睡 (sleep)」10 秒，從 sleep 指令下達到它結束終止共經過 10 秒，所以 real time 的值是 10 秒。但，sleep 指令在 user mode 及 kernel mode 都沒有佔用 CPU，所以 user CPU time 和 system CPU time 基本上都是「零」。

##二：real time > user CPU time + system CPU time

這個式子若是在單核心、單顆的 CPU 上，通常有成立的可能。

但，在多核心或多顆 CPU 的環境，卻往往可能出現 user CPU time + system CPU time > real time 的狀況。甚至我們在編譯大量的程式時，經常執行 time make -j2 或 time make -j4 的指令，往往得到的 user CPU time 值都可能大過 real time 值。

