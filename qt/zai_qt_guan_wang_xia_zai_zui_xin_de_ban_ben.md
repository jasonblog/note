# 在QT官網下載最新的版本


1.在QT官網下載最新的版本http://qt-project.org/downloads<br>
我下的是qt-opensource-linux-x86-5.3.1.run,在終端執行<br>

```sh
sudo chmod a+x qt-opensource-linux-x86-5.3.1.run
./qt-opensource-linux-x86-5.3.1.run
```

然後選擇安裝目錄，默認是/home/username/QT5.3.1,我選擇在/opt/QT5

2.安裝成功後執行qmake -v 會發現找不到qmake命令，這是因為qmake沒有加入到環境變量中<br>
在～/.bashrc中的最後添加<br>
```sh
export PATH="/opt/QT5/5.3/gcc/bin":$PATH
```

然後要將剛加入的環境變量生效，可以註銷之後再登錄，也可以執行

```sh
source ～/.bashrc
```

在執行qmake -v的話，會有

```sh
root@zwq:~# qmake -v
QMake version 3.0
Using Qt version 5.3.1 in /opt/QT5/5.3/gcc/lib
```

3.現在談談在終端中編譯一個簡單的qt程序
代碼為

```c
#include <qapplication.h>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    QWidget *mainwin = new QWidget();
    mainwin->setCaption("Hello!! Qt!!");
    mainwin->resize(200, 150);
    app.setMainWidget(mainwin);

    mainwin->show();

    return app.exec();
}
```
 
使用qmake命令編譯
```sh
qmake -project    
```

這一步如果出現這樣的錯誤
```sh
Failure to open file: /home/username/workspace/c/c.pro
Unable to generate project file.
```

我懷疑是權限問題，於是加了sudo權限，出現這樣的錯誤，
sudo: qmake: command not found
看來root權限用不了qmake這個命令了，但是我直接切換到root之後，又可以用了。編譯成功之後，執行make命令，又提示
```sh
Helloworld.cpp:1:23: fatal error: QApplication: No such file or directory
compilation terminated.
make: *** [Helloworld.o] Error 1
```

這個問題的解決辦法是
在生成的pro文件中添加

```sh
QT+=widgets
```

再次make就可以編譯成功了