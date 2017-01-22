# 在QT官网下载最新的版本


1.在QT官网下载最新的版本http://qt-project.org/downloads<br>
我下的是qt-opensource-linux-x86-5.3.1.run,在终端执行<br>

```sh
sudo chmod a+x qt-opensource-linux-x86-5.3.1.run
./qt-opensource-linux-x86-5.3.1.run
```

然后选择安装目录，默认是/home/username/QT5.3.1,我选择在/opt/QT5

2.安装成功后执行qmake -v 会发现找不到qmake命令，这是因为qmake没有加入到环境变量中<br>
在～/.bashrc中的最后添加<br>
```sh
export PATH="/opt/QT5/5.3/gcc/bin":$PATH
```

然后要将刚加入的环境变量生效，可以注销之后再登录，也可以执行

```sh
source ～/.bashrc
```

在执行qmake -v的话，会有

```sh
root@zwq:~# qmake -v
QMake version 3.0
Using Qt version 5.3.1 in /opt/QT5/5.3/gcc/lib
```

3.现在谈谈在终端中编译一个简单的qt程序
代码为

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
 
使用qmake命令编译
```sh
qmake -project    
```

这一步如果出现这样的错误
```sh
Failure to open file: /home/username/workspace/c/c.pro
Unable to generate project file.
```

我怀疑是权限问题，于是加了sudo权限，出现这样的错误，
sudo: qmake: command not found
看来root权限用不了qmake这个命令了，但是我直接切换到root之后，又可以用了。编译成功之后，执行make命令，又提示
```sh
Helloworld.cpp:1:23: fatal error: QApplication: No such file or directory
compilation terminated.
make: *** [Helloworld.o] Error 1
```

这个问题的解决办法是
在生成的pro文件中添加

```sh
QT+=widgets
```

再次make就可以编译成功了