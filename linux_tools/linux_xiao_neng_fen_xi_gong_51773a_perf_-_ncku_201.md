# Linux 效能分析工具: Perf - NCKU 2015


###安裝相關開發工具
```sh
$ sudo apt-get update
$ sudo apt-get install build-essential
$ sudo apt-get install linux-tools-common linux-tools-generic
$ sudo apt-get install astyle colordiff
```

## Linux 效能分析工具: Perf

Perf 全名是 Performance Event，是在 Linux 2.6.31 以後內建的系統效能分析工具，它隨著核心一併釋出。藉由 perf，應用程式可以利用 PMU (Performance Monitoring Unit), tracepoint 和核心內部的特殊計數器 (counter) 來進行統計，另外還能同時分析運行中的核心程式碼，從而更全面了解應用程式中的效能瓶頸。

相較於 OProfile 和 GProf ，perf 的優勢在於與 Linux 核心緊密結合，並可受益於最先納入核心的新特徵。perf 基本原理是對目標進行取樣，紀錄特定的條件下所偵測的事件Branch Prediction是否發生以及發生的次數。例如根據 tick 中斷進行取樣，即在 tick 中斷內觸發取樣點，在取樣點裡判斷行程 (process) 當時的 context。假如一個行程 90% 的時間都花費在函式 foo() 上，那麼 90% 的取樣點都應該落在函式 foo() 的上下文中。

Perf 可取樣的事件非常多，可以分析 Hardware event，如 cpu-cycles、instructions 、cache-misses、branch-misses …等等。可以分析 Software event，如 page-faults、context-switches …等等，另外一種就是 Tracepoint event。知道了 cpu-cycles、instructions 我們可以了解 Instruction per cycle 是多少，進而判斷程式碼有沒有好好利用 CPU，cache-misses 可以曉得是否有善用 Locality of reference ，branch-misses 多了是否導致嚴重的 pipeline hazard ？另外 Perf 還可以對函式進行採樣，了解效能卡在哪邊。

###Perf 安裝

首先利用以下指令查看目前的 Linux 核心組態是否啟用 perf。如果電腦已安裝常見 GNU/Linux 發行版本，預設值應該都有開啟。

```sh
＄ cat "/boot/config-`uname -r`" | grep "PERF_EVENT"
```

參考的環境是 Ubuntu 14.04 輸入$ perf list 或 $ sudo perf top 檢查一下 perf 可不可以使用。

如果出現以下的訊息，表示還漏了些東西。(下方的核心版本可能和你不一樣，根據指示安裝起來即可。不放心的話可以使用 ＄ uname -r 確認)

```sh
WARNING: perf not found for kernel 3.16.0-50
You may need to install the following packages for this specific kernel:
    linux-tools-3.16.0-50-generic
    linux-cloud-tools-3.16.0-50-generic
```

於是執行以下指令:
```sh
$ sudo apt-get install linux-tools-3.16.0-50-generic linux-cloud-tools-3.16.0-50-generic
```

到這裡 perf 的安裝就完成了。

如果你不是切換到 root 的情況下輸入 $ perf top，會出現以下錯誤畫面：

