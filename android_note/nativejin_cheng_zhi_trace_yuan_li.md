# Native進程之Trace原理

##一. 概述
當發生ANR(Application Not Response，對於`Java進程可通過kill -3` 向目標進程發送信號SIGNAL_QUIT, 輸出相應的traces信息保存到目錄/data/anr/traces.txt；而對於`Native進程可通過 debuggerd` 輸出traces信息。

可通過一條命令來獲取指定Native進程的traces信息，例如輸出pid=17529進程信息：


```sh
adb shell debuggerd -b 17529 //可指定進程pid
```

執行完該命令後直接輸出traces信息到屏幕，如下：


```sh
//【見小節2.2】
----- pid 17529 at 2016-11-12 22:22:22 -----
Cmd line: /system/bin/mediaserver
ABI: 'arm'

//【見小節2.4】
"mediaserver" sysTid=17529
  #00 pc 00042dac /system/lib/libc.so (__ioctl+8)
  #01 pc 000498ad /system/lib/libc.so (ioctl+14)
  #02 pc 0001ea5b /system/lib/libbinder.so (_ZN7android14IPCThreadState14talkWithDriverEb+174)
  #03 pc 0001ef9f /system/lib/libbinder.so (_ZN7android14IPCThreadState20getAndExecuteCommandEv+6)
  #04 pc 0001f0d7 /system/lib/libbinder.so (_ZN7android14IPCThreadState14joinThreadPoolEb+78)
  #05 pc 00001cf1 /system/bin/mediaserver
  #06 pc 0001667d /system/lib/libc.so (__libc_init+44)
  #07 pc 00001f48 /system/bin/mediaserver

"Binder_1" sysTid=17931
  #00 pc 00042dac /system/lib/libc.so (__ioctl+8)
  #01 pc 000498ad /system/lib/libc.so (ioctl+14)
  #02 pc 0001ea5b /system/lib/libbinder.so (_ZN7android14IPCThreadState14talkWithDriverEb+174)
  #03 pc 0001ef9f /system/lib/libbinder.so (_ZN7android14IPCThreadState20getAndExecuteCommandEv+6)
  #04 pc 0001f0d7 /system/lib/libbinder.so (_ZN7android14IPCThreadState14joinThreadPoolEb+78)
  #05 pc 00023bd1 /system/lib/libbinder.so
  #06 pc 00010115 /system/lib/libutils.so (_ZN7android6Thread11_threadLoopEPv+112)
  #07 pc 00041843 /system/lib/libc.so (_ZL15__pthread_startPv+30)
  #08 pc 000192a5 /system/lib/libc.so (__start_thread+6)
... //此處省略剩餘的N個線程.
----- end 17529 -----
```
接下來說說debuggerd是如何輸出Native進程的trace.

##二. Debuggerd
文章debuggerd守護進程詳細介紹了Debuggerd的工作原理，此處當執行`debuggerd -b`命令後：

- Client進程調用send_request()方法向debuggerd服務端發出`DEBUGGER_ACTION_DUMP_BACKTRACE`命令；
- Debugggerd進程收到該命令，fork子進程中再執行worker_process()過程；
- 子進程通過perform_dump()方法來根據命令`DEBUGGER_ACTION_DUMP_BACKTRACE`，會調用到dump_backtrace()方法輸出backtrace.
接下來，從dump_backtrace()方法講起：

### 2.1 dump_backtrace

- debuggerd/backtrace.cpp


```cpp
void dump_backtrace(int fd, BacktraceMap* map, pid_t pid, pid_t tid,
                    const std::set<pid_t>& siblings, std::string* amfd_data)
{
    log_t log;
    log.tfd = fd;
    log.amfd_data = amfd_data;

    dump_process_header(&log, pid); //【見小節2.2】
    dump_thread(&log, map, pid, tid);//【見小節2.3】

    for (pid_t sibling : siblings) {
        dump_thread(&log, map, pid, sibling);//【見小節2.3】
    }

    dump_process_footer(&log, pid);//【見小節2.4】
}
```

### .2 dump_process_header

- debuggerd/backtrace.cpp

```cpp
static void dump_process_header(log_t* log, pid_t pid)
{
    char path[PATH_MAX];
    char procnamebuf[1024];
    char* procname = NULL;
    FILE* fp;

    //獲取/proc/<pid>/cmdline節點的進程名
    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);

    if ((fp = fopen(path, "r"))) {
        procname = fgets(procnamebuf, sizeof(procnamebuf), fp);
        fclose(fp);
    }

    time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);
    char timestr[64];
    strftime(timestr, sizeof(timestr), "%F %T", &tm);
    _LOG(log, logtype::BACKTRACE, "\n\n----- pid %d at %s -----\n", pid, timestr);

    if (procname) {
        _LOG(log, logtype::BACKTRACE, "Cmd line: %s\n", procname);
    }

    _LOG(log, logtype::BACKTRACE, "ABI: '%s'\n", ABI_STRING);
}
```

例如：

```sh
----- pid 17529 at 2016-11-12 22:22:22 -----
Cmd line: /system/bin/mediaserver
ABI: 'arm'
```

### 2.3 dump_thread

-  debuggerd/backtrace.cpp


```cpp
static void dump_thread(log_t* log, BacktraceMap* map, pid_t pid, pid_t tid)
{
    char path[PATH_MAX];
    char threadnamebuf[1024];
    char* threadname = NULL;
    FILE* fp;

    //獲取/proc/<tid>/comm節點的線程名
    snprintf(path, sizeof(path), "/proc/%d/comm", tid);

    if ((fp = fopen(path, "r"))) {
        threadname = fgets(threadnamebuf, sizeof(threadnamebuf), fp);
        fclose(fp);

        if (threadname) {
            size_t len = strlen(threadname);

            if (len && threadname[len - 1] == '\n') {
                threadname[len - 1] = '\0';
            }
        }
    }

    _LOG(log, logtype::BACKTRACE, "\n\"%s\" sysTid=%d\n",
         threadname ? threadname : "<unknown>", tid);

    std::unique_ptr<Backtrace> backtrace(Backtrace::Create(pid, tid, map));

    if (backtrace->Unwind(0)) {
        //【見小節2.4】
        dump_backtrace_to_log(backtrace.get(), log, " ");
    }
}
```

### 2.4 dump_backtrace_to_log

- debuggerd/Backtrace.cpp


```cpp
void dump_backtrace_to_log(Backtrace* backtrace, log_t* log,
                           const char* prefix)
{
    //NumFrames是backtrace中的棧幀數
    for (size_t i = 0; i < backtrace->NumFrames(); i++) {
        //【見小節2.5】
        _LOG(log, logtype::BACKTRACE, "%s%s\n", prefix,
             backtrace->FormatFrameData(i).c_str());
    }
}
```

通過循環遍歷輸出整個backtrace中的每一棧幀FormatFrameData的信息.


### 2.5 FormatFrameData


- debuggerd/Backtrace.cpp

```cpp
std::string Backtrace::FormatFrameData(size_t frame_num)
{
    if (frame_num >= frames_.size()) {
        return "";
    }

    return FormatFrameData(&frames_[frame_num]);
}

std::string Backtrace::FormatFrameData(const backtrace_frame_data_t* frame)
{
    const char* map_name;

    if (BacktraceMap::IsValid(frame->map) && !frame->map.name.empty()) {
        map_name = frame->map.name.c_str();
    } else {
        map_name = "<unknown>";
    }

    uintptr_t relative_pc = BacktraceMap::GetRelativePc(frame->map, frame->pc);

    //這是backtrace每一行的信息
    std::string line(StringPrintf("#%02zu pc %" PRIPTR " %s",
                                  frame->num, relative_pc, map_name));

    if (frame->map.offset != 0) {
        line += " (offset " + StringPrintf("0x%" PRIxPTR, frame->map.offset) + ")";
    }

    if (!frame->func_name.empty()) {
        //函數名，偏移量
        line += " (" + frame->func_name;

        if (frame->func_offset) {
            line += StringPrintf("+%" PRIuPTR, frame->func_offset);
        }

        line += ')';
    }

    return line;
}
```
例如：(這些map信息是由/proc/%d/maps解析出來的)

```sh
#01 pc 000000000001cca4 /system/lib64/libc.so (epoll_pwait+32)
```

<tbody>
    <tr>
      <td>幀號</td>
      <td>pc指針</td>
      <td>map_name</td>
      <td>(函數名+偏移量)</td>
    </tr>
    <tr>
      <td>#01</td>
      <td>pc 000000000001cca4</td>
      <td>/system/lib64/libc.so</td>
      <td>(epoll_pwait+32)</td>
    </tr>
  </tbody>
  
  
### 2.6 dump_process_footer

- debuggerd/backtrace.cpp


```cpp
static void dump_process_footer(log_t* log, pid_t pid)
{
    _LOG(log, logtype::BACKTRACE, "\n----- end %d -----\n", pid);
}
```

例如：----- end 1789 -----


## 三. 總結

通過debuggerd -b [pid]，可輸出Native進程的調用棧，這些信息是通過解析`/proc/[pid]/maps`而來的。
