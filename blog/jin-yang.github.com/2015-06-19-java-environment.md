---
title: Java 環境搭建
layout: post
comments: true
language: chinese
category: [program]
keywords: java,環境搭建
description: 簡單記錄一下 Linux 下的 Java 環境搭建，以及使用方法。
---

簡單記錄一下 Linux 下的 Java 環境搭建，以及使用方法。

<!-- more -->

## 簡介

可以通過 ```rpm -qa | grep java``` 查看是否已經安裝 java 包，CentOS 默認安裝的是 ```openjdk```，常用的 ```openjdk```、```openjdk-headless```、```tzdata-java``` 已經安裝，通過 ```java -version``` 查看版本號。

將會將代碼安裝在 ```/usr/lib/jvm/java-1.7.0-openjdk-1.7.0.65-2.5.1.2.el7_0.x86_64/jre/``` 目錄下。

可以刪除 openjdk 然後安裝 Sun，不太建議，安裝 devel 包，含有 ```jar``` ```jarsigner``` ```javac``` ```javadoc``` ```javah``` ```javap``` 等軟件。

{% highlight java %}
public class HelloWorld {
    public static void main(String[] args) {
        System.out.println("Hello! World!");
    }
}
{% endhighlight %}

文件名必須與類相同，即 ```HelloWorld.java``` ，通過 ```javac HelloWorld.java``` 編譯，```java HelloWorld``` 運行。

### JAR 包

Java Archive File, JAR 包是 Java 的一種文檔格式，實際上就是 ZIP 文件，唯一的區別就是在 JAR 文件的內容中，包含了一個 ```META-INF/MANIFEST.MF``` 文件，該文件是在生成 JAR 時自動創建。

當包含一個 jar 包時，如果不知道所包含的類，可以通過解壓包查看。

Java 程序由若干個 `.class` 文件組成的，這些文件必須根據它們所屬的包不同而分級分目錄存放，運行前需要把所有用到的包的根目錄指定給 `CLASSPATH` 環境變量或者 java 命令的 `-cp` 參數。

<!--
創建可執行的 JAR 文件包，需要使用帶 cvfm 參數的 jar 命令，同樣以上述 test 目錄為例，命令如下：
jar cvfm test.jar manifest.mf test
這裡 test.jar 和 manifest.mf 兩個文件，分別是對應的參數 f 和 m，其重頭戲在 manifest.mf。因為要創建可執行的 JAR 文件包，光靠指定一個 manifest.mf 文件是不夠的，因為 MANIFEST 是 JAR 文件包的特徵，可執行的 JAR 文件包和不可執行的 JAR 文件包都包含 MANIFEST。關鍵在於可執行 JAR 文件包的 MANIFEST，其內容包含了 Main-Class 一項。這在 MANIFEST 中書寫格式如下：

Main-Class: 可執行主類全名(包含包名)
例如，假設上例中的 Test.class 是屬於 test 包的，而且是可執行的類 (定義了 public static void main(String[]) 方法)，那麼這個 manifest.mf 可以編輯如下：

Main-Class: test.Test <回車>
這個 manifest.mf 可以放在任何位置，也可以是其它的文件名，只需要有 Main-Class: test.Test 一行，且該行以一個回車符結束即可。創建了 manifest.mf 文件之後，我們的目錄結構變為：

　　==
　　|- test
　　|`- Test.class
　　`- manifest.mf

這時候，需要到 test 目錄的上級目錄中去使用 jar 命令來創建 JAR 文件包。也就是在目錄樹中使用“==”表示的那個目錄中，使用如下命令：

jar cvfm test.jar manifest.mf test
之後在“==”目錄中創建了 test.jar，這個 test.jar 就是執行的 JAR 文件包。運行時只需要使用 java -jar test.jar 命令即可。

需要注意的是，創建的 JAR 文件包中需要包含完整的、與 Java 程序的包結構對應的目錄結構，就像上例一樣。而 Main-Class 指定的類，也必須是完整的、包含包路徑的類名，如上例的 test.Test；而且在沒有打成 JAR 文件包之前可以使用 java <類名> 來運行這個類，即在上例中 java test.Test 是可以正確運行的 (當然要在 CLASSPATH 正確的情況下)。
-->


{% highlight text %}
$ tree
$ jar -cvf hello.jar helloworld
$ jar -tvf hello.jar                 // 查看JAR包的詳細信息
$ jar -xf hello.jar                  // 解壓
{% endhighlight %}

在 eclipse 可以直接通過 export 打包。

很多 Java 內部函數在 ```java-x.x.x-openjdk-headless``` 中定義。

{% highlight text %}
$ rpm -ql java-1.7.0-openjdk-headless-1.7.0.95-2.6.4.0.el7_2.x86_64 | grep -P '\.jar$' > /tmp/1
$ for i in `cat /tmp/1`; do ls -alh $i; done                                               # 部分文件是符號鏈接
$ vi /tmp/1                                                                                # 去除符號鏈接
$ for i in `cat /tmp/1`; do echo $i && jar -tvf $i | grep DriverManager; done              # 查看相應的接口
{% endhighlight %}


## 安裝 Eclipse

另一個比較好用的開發環境是 [IDEA](https://www.jetbrains.com/idea/) 。

### 安裝

在 CentOS 中，可以通過如下步驟安裝。

1. 從 [www.eclipse.org](http://www.eclipse.org/downloads/) 下載最新的 Eclipse，通常為 ```eclipse-xxxx-linux-gtk-x86_64.tar.gz``` 。

2. 解壓到 ```/opt``` 目錄下，```su - && tar -xvzf eclipse-xxx-linux-gtk.tar.gz -C /opt && ln -s /usr/eclipse/eclipse /usr/bin/eclipse```，此時可以在 Terminal 中通過 eclipse 命令啟動。

3. 添加文件 ```vi /usr/share/applications/eclipse-4.4.desktop```，於是可在左上角找到 Eclipse 。

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
最後一步也可以創建一個可執行腳本(沒有試過)，內容為：
#!/bin/sh
export ECLIPSE_HOME="/opt/eclipse"
$ECLIPSE_HOME/eclipse $*

使用方法可以參考 《使用Eclipse構建Maven項目》
-->

maven 工程創建需要一個 groupid 和 artifactid，兩者的組合需要全局唯一，一般 groupid 是包名，artifactid 為工程名 (會在 Eclipse 中展示)。

### HelloWorld

可以通過如下步驟簡單創建一個 Java 項目。

#### 1. 新建 Java 項目

選擇 `"File → New → Project..."`，選擇 `"Java Project"`，便打開了 `"New Java Project"` 嚮導。在 `"Project name"` 中填入 `"HelloWorld"`，直接點擊 `"Finish"` 按鈕。

#### 2. 新建一個 Package

值得注意的是包名和工程名一般都是小寫開頭，而 Java 類名則是大寫開頭。選擇 `"File → New → Package"`，在 `"New Java Package"` 嚮導中的 Name 框中輸入 `"test"`，直接 finish。

#### 3. 新建 HelloWorldApp 類

選擇 `"File → New → Class"`，在 `"New Java Class"` 嚮導中的 Name 框中輸入 `"HelloWorldApp"`，並且在 `"public static void main(String[] args)"` 選項前面打上勾；在 Source floder 中選擇上步填寫的項目。

#### 4. 添加打印語句

在 main 中添加如下內容。

{% highlight text %}
System.out.println("Hello World!");
{% endhighlight %}

#### 5. 運行 Java 程序

按下 `"Run → Run..."`，Eclipse 會彈出運行設置嚮導，選擇 `"Java Application"` 。

#### 其它

然後可以右健點擊對應文件，`"Export... → Runnable JAR file"`，在 `Launch configuration` 中選擇對應的項目，並在 `Library handling` 中選擇 `Extract required libraries into generated JAR` 。

接著通過 `java -jar helloworld.jar` 就可以運行。

### 安裝 JDBC

在 CentOS 中可以通過 `yum install mysql-connector-java` 安裝，此時會保存在 `/usr/share/java/mysql-connector-java.jar` ，如果要使用這個包需要包含全路徑，可以設置 `CLASSPATH` 環境變量，或者：

{% highlight text %}
java -classpath .:/usr/share/java/mysql-connector-java.jar GetConnection
{% endhighlight %}

在 Eclipse 中可通過 `[右健] -> Properties -> Java Build Path -> Libraries -> Add  External JARs` 。

可以從 MySQL 官網下載 [MySQL Connectors](http://www.mysql.com/products/connector/) 。

### 安裝 Maven

在 CentOS 中也可以 `yum install maven` 安裝，或者從官網直接下載源碼。

從 [maven.apache.org](http://maven.apache.org/) 下載 Maven-bin ，解壓，可以放置到任何目錄下；新建符號連接，方便升級 `ln -s apache-maven-3.2.3 apache-maven` 。

在 `~/.bashrc` 文件中添加環境變量，然後通過 `source ~/.bashrc` 更新:

{% highlight text %}
export M2_HOME=/home/andy/Workspace/java/tools/apache-maven
PATH=$PATH:$M2_HOME/bin
{% endhighlight %}

通過執行 `mvn -v` 查看是否安裝成功，此時會先是 Java 的版本號等信息。

在 eclipse 中設置 maven，`Window -> Preference -> Maven -> {Installations, User Settings}`，配置文件可以統一使用集團的，可以設置全局以及用戶的，此時會自動生成 repository 目錄。

#### 新建工程

可以通過如下的方法在 Eclipse 中創建一個最簡單的 Maven 工程，步驟如下 `File -> New -> Project... -> Maven[Maven Project] -> Next... -> 選擇 quickstart` 即可。

然後點擊運行，可以看到一個最簡單的 `Hello World` 程序，如果出現 `Missing artifact`，修改後可以通過 `[右鍵] -> Maven -> Update Project...` 即可。

創建 mvn 工程，也可以執行如下命令創建一個 maven 工程，然後導入。

{% highlight text %}
$ mvn archetype:generate -DgroupId=abc -DartifactId=hello \
    -DarchetypeArtifactId=maven-archetype-quickstart -DinteractiveMode=false
{% endhighlight %}

## 參考

<!--
<br><br><h2>閱讀源碼</h2><p>
如果通過 eclipse 閱讀源碼，可以直接新建 Java Project，然後將源碼目錄設置為相應的目錄。或者在相應的 java 包上點擊右健，選擇 Properties，會出現 Java Source Attachment 對話框，然後選擇 External location -&gt; External Floder... 選擇相應的目錄即可。<br><br>

常用快捷鍵。<ul><li>
Ctrl-左健<br>
查看變量、方法、類的定義。</li><br><li>

Ctrl+O

查看一個類的綱要，列出其方法和成員變量。提示：再多按一次Ctrl+O，可以列出該類繼承的方法和變量。

助記："O"---"Outline"---"綱要"

3. Ctrl+T

查看一個類的繼承關係樹，是自頂向下的，再多按一次Ctrl+T, 會換成自底向上的顯示結構。

提示：選中一個方法名，按Ctrl+T，可以查看到有這個同名方法的父類、子類、接口。

助記："T"-------"Tree"-----"層次樹"

4.Alt+左右方向鍵

我們經常會遇到看代碼時Ctrl+左鍵，層層跟蹤，然後迷失在代碼中的情況，這時只需要按“Alt+左方向鍵”就可以退回到上次閱讀的位置，同理，按“Alt+右方向鍵”會前進到剛才退回的閱讀位置，就像瀏覽器的前進和後退按鈕一樣。

5.Ctrl+Alt+H

如果你想知道一個類的方法到底被那些其他的類調用，那麼請選中這個方法名，然後按“Ctrl+Alt+H”，Eclipse就會顯示出這個方法被哪些方法調用，最終產生一個調用關係樹。

助記："H"---"Hierarchy"---"調用層次"



Antlr，一個特定領域編程語言 (Domain Specific Languages, DSL)
Groovy入門教程，據說是下一代的java語言，同樣運行在 JVM 中。

http://www.infoq.com/cn/java-depth-adventure
-->


{% highlight text %}
{% endhighlight %}
