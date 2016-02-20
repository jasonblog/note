# linux系统编程之进程（四）：进程退出exit，_exit区别即atexit函数


## 一，进程终止有5种方式：

- 正常退出：
    - 从main函数返回
    - 调用exit
    - 调用_exit


- 异常退出：
    - 调用abort
    - 由信号终止

##二，exit和_exit区别：

