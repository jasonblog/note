# 如何了解Task、Thread、ThreadPool、Handler、Looper、Message、MessageQueue、AsyncTask


Task、Thread、ThreadPool、Handler、Looper、Message、MessageQueue、AsyncTask
這邊一開始會很混亂, 試圖用一個例子來解說每個所代表的腳色,
想來想去覺得電影院蠻適合的

```sh
Task就是你要看的電影
Thread就是人
ThreadPool就是電影廳 
Looper就是售票員
Message就是門票
MessageQueue就是出票機
Handler就是剪票員 
AsyncTask是電影院
```

你想要去看電影(執行task)的話, 
就要先去電影院找售票員(Looper), 售票員從出票機(MessageQueue, 一次一張循序)就會給你一張電影票(得到Message),
剪票員(Handler)檢查過你的電影票(Message)確定是這個電影廳就讓你去看電影(執行Task)。

電影院(AsyncTask)有很多電影廳(ThreadPool), 每一個電影廳可以裝很多人(Threads)看電影(執行Task),
也可以一個人(single thread)看一場電影(執行Task)。