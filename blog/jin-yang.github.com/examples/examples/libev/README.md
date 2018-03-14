
libev 庫的示例程序。

### example.c

簡單程序，要麼等待5秒超時，或者等待stdin輸入字符，例如Ctrl-D、輸入字符+回車。


### timer.c

定時器示例程序，包括了三個定時器，默認在初始化時會設置開始時間+重複時間間隔；如果不設置開始時間，則會在開始立即執行一次。

      start at 1492353120
    oneshot at 1492353122
    repeate at 1492353125
    repeate at 1492353126
    repeate at 1492353127
    repeate at 1492353128
    repeate at 1492353129
    repeate at 1492353130
    timeout at 1492353130

### signal.c

等待SIGINT信號，可以通過kill -SIGINT PID發送信號。

### child.c

創建一個子進程，然後主進程中等待子進程退出；而子進程會sleep 1秒後退出。

### filestat.c

監控Makefile文件的狀態，可以通過```touch Makefile```修改文件的屬性。

### periodic.c

其中示例可以查看上述源碼文件中的註釋。

### misc.c

輸出libev庫的版本，然後sleep 2.3秒，主要是一些常用的函數調用。
