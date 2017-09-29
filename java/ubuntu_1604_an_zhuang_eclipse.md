# Ubuntu 16.04 安装 Eclipse


##1 安装Java JDK

##2 下载Eclipse安装器

http://www.eclipse.org/downloads/；根据系统类型下载：


![](images/Screen-Shot-2016-05-06-at-09.57.57.png)
## 3 解压并运行其中的eclipse-inst


```sh
$ tar xzvf eclipse-inst-linux64.tar.gz
$ cd eclipse-installer
$ ./eclipse-inst
```

选择你要安装eclipse的类型：

![](images/Screen-Shot-2016-05-06-at-10.04.49.png)
我选择java开发环境；

选择安装路径，开始安装：

![](images/Screen-Shot-2016-05-06-at-10.06.03.png)

##4 在Unity Dash中创建一个快捷菜单项

创建文件：

```sh
sudo vim /usr/share/applications/eclipse.desktop 
```

```
[Desktop Entry]
Name=Eclipse
Type=Application
Exec=/home/shihyu/eclipse/java-oxygen/eclipse/eclipse
Terminal=false
Icon=/home/shihyu/eclipse/java-oxygen/eclipse/icon.xpm
Comment=Integrated Development Environment
NoDisplay=false
Categories=Development;IDE;
Name[en]=Eclipse
```
把上面的/home/snail/eclipse/…替换为你的eclipse安装路径。

注销系统。



![](images/Screen-Shot-2016-05-06-at-10.35.44.png)