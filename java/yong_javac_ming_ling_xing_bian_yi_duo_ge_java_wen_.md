# 用javac命令行编译多个java文件


CLASSPATH一定要具体到jar路径，例如

```sh
export CLASSPATH=/usr/lib/jvm/java-6-sun/lib/commons-codec.jar:/usr/lib/jvm/java-6-sun/lib/commons-httpclient-3.1.jar:/usr/lib/jvm/java-6-sun/lib/commons-logging-1.1.jar:/usr/lib/jvm/java-6-sun/lib/dt.jar:/usr/lib/jvm/java-6-sun/lib/htmlconverter.jar:/usr/lib/jvm/java-6-sun/lib/jconsole.jar:/usr/lib/jvm/java-6-sun/lib/junit-4.1.jar:/usr/lib/jvm/java-6-sun/lib/mysql-connector-java-5.1.16-bin.jar:/usr/lib/jvm/java-6-sun/lib/sa-jdi.jar:/usr/lib/jvm/java-6-sun/lib/tools.jar
```

每个java文件给出路径，最常见的是用通配符*，不支持目录递归。

最好加上 -d，这样会在这个目录下生成class文件，而不会和java文件混在一起

看我的一个具体的例子


```sh
javac -d /home/dfq/crawler/bin crawler/*.java weibo4j/*.java weibo4j/org/json/*.java weibo4j/*.java weibo4j/util/*.java weibo4j/http/*.java
```

运行时用如下命令：


```sh
cd /home/dfq/crawler/bin
java -cp .:$CLASSPATH crawler.ManagerThread
```


