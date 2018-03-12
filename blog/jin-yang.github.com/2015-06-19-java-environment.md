---
title: Java 环境搭建
layout: post
comments: true
language: chinese
category: [program]
keywords: java,环境搭建
description: 简单记录一下 Linux 下的 Java 环境搭建，以及使用方法。
---

简单记录一下 Linux 下的 Java 环境搭建，以及使用方法。

<!-- more -->

## 简介

可以通过 ```rpm -qa | grep java``` 查看是否已经安装 java 包，CentOS 默认安装的是 ```openjdk```，常用的 ```openjdk```、```openjdk-headless```、```tzdata-java``` 已经安装，通过 ```java -version``` 查看版本号。

将会将代码安装在 ```/usr/lib/jvm/java-1.7.0-openjdk-1.7.0.65-2.5.1.2.el7_0.x86_64/jre/``` 目录下。

可以删除 openjdk 然后安装 Sun，不太建议，安装 devel 包，含有 ```jar``` ```jarsigner``` ```javac``` ```javadoc``` ```javah``` ```javap``` 等软件。

{% highlight java %}
public class HelloWorld {
    public static void main(String[] args) {
        System.out.println("Hello! World!");
    }
}
{% endhighlight %}

文件名必须与类相同，即 ```HelloWorld.java``` ，通过 ```javac HelloWorld.java``` 编译，```java HelloWorld``` 运行。

### JAR 包

Java Archive File, JAR 包是 Java 的一种文档格式，实际上就是 ZIP 文件，唯一的区别就是在 JAR 文件的内容中，包含了一个 ```META-INF/MANIFEST.MF``` 文件，该文件是在生成 JAR 时自动创建。

当包含一个 jar 包时，如果不知道所包含的类，可以通过解压包查看。

Java 程序由若干个 `.class` 文件组成的，这些文件必须根据它们所属的包不同而分级分目录存放，运行前需要把所有用到的包的根目录指定给 `CLASSPATH` 环境变量或者 java 命令的 `-cp` 参数。

<!--
创建可执行的 JAR 文件包，需要使用带 cvfm 参数的 jar 命令，同样以上述 test 目录为例，命令如下：
jar cvfm test.jar manifest.mf test
这里 test.jar 和 manifest.mf 两个文件，分别是对应的参数 f 和 m，其重头戏在 manifest.mf。因为要创建可执行的 JAR 文件包，光靠指定一个 manifest.mf 文件是不够的，因为 MANIFEST 是 JAR 文件包的特征，可执行的 JAR 文件包和不可执行的 JAR 文件包都包含 MANIFEST。关键在于可执行 JAR 文件包的 MANIFEST，其内容包含了 Main-Class 一项。这在 MANIFEST 中书写格式如下：

Main-Class: 可执行主类全名(包含包名)
例如，假设上例中的 Test.class 是属于 test 包的，而且是可执行的类 (定义了 public static void main(String[]) 方法)，那么这个 manifest.mf 可以编辑如下：

Main-Class: test.Test <回车>
这个 manifest.mf 可以放在任何位置，也可以是其它的文件名，只需要有 Main-Class: test.Test 一行，且该行以一个回车符结束即可。创建了 manifest.mf 文件之后，我们的目录结构变为：

　　==
　　|- test
　　|`- Test.class
　　`- manifest.mf

这时候，需要到 test 目录的上级目录中去使用 jar 命令来创建 JAR 文件包。也就是在目录树中使用“==”表示的那个目录中，使用如下命令：

jar cvfm test.jar manifest.mf test
之后在“==”目录中创建了 test.jar，这个 test.jar 就是执行的 JAR 文件包。运行时只需要使用 java -jar test.jar 命令即可。

需要注意的是，创建的 JAR 文件包中需要包含完整的、与 Java 程序的包结构对应的目录结构，就像上例一样。而 Main-Class 指定的类，也必须是完整的、包含包路径的类名，如上例的 test.Test；而且在没有打成 JAR 文件包之前可以使用 java <类名> 来运行这个类，即在上例中 java test.Test 是可以正确运行的 (当然要在 CLASSPATH 正确的情况下)。
-->


{% highlight text %}
$ tree
$ jar -cvf hello.jar helloworld
$ jar -tvf hello.jar                 // 查看JAR包的详细信息
$ jar -xf hello.jar                  // 解压
{% endhighlight %}

在 eclipse 可以直接通过 export 打包。

很多 Java 内部函数在 ```java-x.x.x-openjdk-headless``` 中定义。

{% highlight text %}
$ rpm -ql java-1.7.0-openjdk-headless-1.7.0.95-2.6.4.0.el7_2.x86_64 | grep -P '\.jar$' > /tmp/1
$ for i in `cat /tmp/1`; do ls -alh $i; done                                               # 部分文件是符号链接
$ vi /tmp/1                                                                                # 去除符号链接
$ for i in `cat /tmp/1`; do echo $i && jar -tvf $i | grep DriverManager; done              # 查看相应的接口
{% endhighlight %}


## 安装 Eclipse

另一个比较好用的开发环境是 [IDEA](https://www.jetbrains.com/idea/) 。

### 安装

在 CentOS 中，可以通过如下步骤安装。

1. 从 [www.eclipse.org](http://www.eclipse.org/downloads/) 下载最新的 Eclipse，通常为 ```eclipse-xxxx-linux-gtk-x86_64.tar.gz``` 。

2. 解压到 ```/opt``` 目录下，```su - && tar -xvzf eclipse-xxx-linux-gtk.tar.gz -C /opt && ln -s /usr/eclipse/eclipse /usr/bin/eclipse```，此时可以在 Terminal 中通过 eclipse 命令启动。

3. 添加文件 ```vi /usr/share/applications/eclipse-4.4.desktop```，于是可在左上角找到 Eclipse 。

{% highlight text %}
[Desktop Entry]
Encoding=UTF-8
Name=Eclipse 4.4.1
Comment=Eclipse Luna
Exec=/usr/bin/eclipse
Icon=/opt/eclipse/icon.xpm
Categories=Application;Development;Java;IDE
Version=1.0
Type=Application
Terminal=0
{% endhighlight %}

<!--
最后一步也可以创建一个可执行脚本(没有试过)，内容为：
#!/bin/sh
export ECLIPSE_HOME="/opt/eclipse"
$ECLIPSE_HOME/eclipse $*

使用方法可以参考 《使用Eclipse构建Maven项目》
-->

maven 工程创建需要一个 groupid 和 artifactid，两者的组合需要全局唯一，一般 groupid 是包名，artifactid 为工程名 (会在 Eclipse 中展示)。

### HelloWorld

可以通过如下步骤简单创建一个 Java 项目。

#### 1. 新建 Java 项目

选择 `"File → New → Project..."`，选择 `"Java Project"`，便打开了 `"New Java Project"` 向导。在 `"Project name"` 中填入 `"HelloWorld"`，直接点击 `"Finish"` 按钮。

#### 2. 新建一个 Package

值得注意的是包名和工程名一般都是小写开头，而 Java 类名则是大写开头。选择 `"File → New → Package"`，在 `"New Java Package"` 向导中的 Name 框中输入 `"test"`，直接 finish。

#### 3. 新建 HelloWorldApp 类

选择 `"File → New → Class"`，在 `"New Java Class"` 向导中的 Name 框中输入 `"HelloWorldApp"`，并且在 `"public static void main(String[] args)"` 选项前面打上勾；在 Source floder 中选择上步填写的项目。

#### 4. 添加打印语句

在 main 中添加如下内容。

{% highlight text %}
System.out.println("Hello World!");
{% endhighlight %}

#### 5. 运行 Java 程序

按下 `"Run → Run..."`，Eclipse 会弹出运行设置向导，选择 `"Java Application"` 。

#### 其它

然后可以右健点击对应文件，`"Export... → Runnable JAR file"`，在 `Launch configuration` 中选择对应的项目，并在 `Library handling` 中选择 `Extract required libraries into generated JAR` 。

接着通过 `java -jar helloworld.jar` 就可以运行。

### 安装 JDBC

在 CentOS 中可以通过 `yum install mysql-connector-java` 安装，此时会保存在 `/usr/share/java/mysql-connector-java.jar` ，如果要使用这个包需要包含全路径，可以设置 `CLASSPATH` 环境变量，或者：

{% highlight text %}
java -classpath .:/usr/share/java/mysql-connector-java.jar GetConnection
{% endhighlight %}

在 Eclipse 中可通过 `[右健] -> Properties -> Java Build Path -> Libraries -> Add  External JARs` 。

可以从 MySQL 官网下载 [MySQL Connectors](http://www.mysql.com/products/connector/) 。

### 安装 Maven

在 CentOS 中也可以 `yum install maven` 安装，或者从官网直接下载源码。

从 [maven.apache.org](http://maven.apache.org/) 下载 Maven-bin ，解压，可以放置到任何目录下；新建符号连接，方便升级 `ln -s apache-maven-3.2.3 apache-maven` 。

在 `~/.bashrc` 文件中添加环境变量，然后通过 `source ~/.bashrc` 更新:

{% highlight text %}
export M2_HOME=/home/andy/Workspace/java/tools/apache-maven
PATH=$PATH:$M2_HOME/bin
{% endhighlight %}

通过执行 `mvn -v` 查看是否安装成功，此时会先是 Java 的版本号等信息。

在 eclipse 中设置 maven，`Window -> Preference -> Maven -> {Installations, User Settings}`，配置文件可以统一使用集团的，可以设置全局以及用户的，此时会自动生成 repository 目录。

#### 新建工程

可以通过如下的方法在 Eclipse 中创建一个最简单的 Maven 工程，步骤如下 `File -> New -> Project... -> Maven[Maven Project] -> Next... -> 选择 quickstart` 即可。

然后点击运行，可以看到一个最简单的 `Hello World` 程序，如果出现 `Missing artifact`，修改后可以通过 `[右键] -> Maven -> Update Project...` 即可。

创建 mvn 工程，也可以执行如下命令创建一个 maven 工程，然后导入。

{% highlight text %}
$ mvn archetype:generate -DgroupId=abc -DartifactId=hello \
    -DarchetypeArtifactId=maven-archetype-quickstart -DinteractiveMode=false
{% endhighlight %}

## 参考

<!--
<br><br><h2>阅读源码</h2><p>
如果通过 eclipse 阅读源码，可以直接新建 Java Project，然后将源码目录设置为相应的目录。或者在相应的 java 包上点击右健，选择 Properties，会出现 Java Source Attachment 对话框，然后选择 External location -&gt; External Floder... 选择相应的目录即可。<br><br>

常用快捷键。<ul><li>
Ctrl-左健<br>
查看变量、方法、类的定义。</li><br><li>

Ctrl+O

查看一个类的纲要，列出其方法和成员变量。提示：再多按一次Ctrl+O，可以列出该类继承的方法和变量。

助记："O"---"Outline"---"纲要"

3. Ctrl+T

查看一个类的继承关系树，是自顶向下的，再多按一次Ctrl+T, 会换成自底向上的显示结构。

提示：选中一个方法名，按Ctrl+T，可以查看到有这个同名方法的父类、子类、接口。

助记："T"-------"Tree"-----"层次树"

4.Alt+左右方向键

我们经常会遇到看代码时Ctrl+左键，层层跟踪，然后迷失在代码中的情况，这时只需要按“Alt+左方向键”就可以退回到上次阅读的位置，同理，按“Alt+右方向键”会前进到刚才退回的阅读位置，就像浏览器的前进和后退按钮一样。

5.Ctrl+Alt+H

如果你想知道一个类的方法到底被那些其他的类调用，那么请选中这个方法名，然后按“Ctrl+Alt+H”，Eclipse就会显示出这个方法被哪些方法调用，最终产生一个调用关系树。

助记："H"---"Hierarchy"---"调用层次"



Antlr，一个特定领域编程语言 (Domain Specific Languages, DSL)
Groovy入门教程，据说是下一代的java语言，同样运行在 JVM 中。

http://www.infoq.com/cn/java-depth-adventure
-->


{% highlight text %}
{% endhighlight %}
