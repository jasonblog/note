## Kotlin

**Kotlin**是一種在[Java虛擬機](https://zh.wikipedia.org/wiki/Java虛擬機)上執行的[靜態型別](https://zh.wikipedia.org/wiki/静态类型)[程式語言](https://zh.wikipedia.org/wiki/编程语言)，它也可以被編譯成為[JavaScript](https://zh.wikipedia.org/wiki/JavaScript)原始碼。它主要是由[俄羅斯](https://zh.wikipedia.org/wiki/俄羅斯)[聖彼得堡](https://zh.wikipedia.org/wiki/聖彼得堡)的[JetBrains](https://zh.wikipedia.org/wiki/JetBrains)開發團隊所發展出來的程式語言，其名稱來自於聖彼得堡附近的[科特林島](https://zh.wikipedia.org/wiki/科特林島)。[[1\]](https://zh.wikipedia.org/wiki/Kotlin#cite_note-oracle_interview-1)2012年1月，著名期刊《[Dr. Dobb's Journal](https://zh.wikipedia.org/w/index.php?title=Dr._Dobb's_Journal&action=edit&redlink=1)》中Kotlin被認定為該月的最佳語言。[[2\]](https://zh.wikipedia.org/wiki/Kotlin#cite_note-dobbs-2)雖然與Java語法並不相容，但在[JVM](https://zh.wikipedia.org/wiki/JVM)環境中Kotlin被設計成可以和Java程式碼相互運作，並可以重複使用如[Java集合框架](https://zh.wikipedia.org/wiki/Java集合框架)等的現有[Java參照的函式庫](https://zh.wikipedia.org/w/index.php?title=Java引用的函数库&action=edit&redlink=1)。Hathibelagal寫道，「如果你正在為Android開發尋找一種替代程式語言，那麼應該試下Kotlin。它很容易在Android專案中替代Java或者同Java一起使用。」



## How to install Kotlin on Ubuntu 20.04 Focal Fossa Linux



6 May 2020 by [Lubos Rendek](https://linuxconfig.org/author/lubos)

Kotlin is a general-purpose programming language which interoperates fully with Java. Kotlin’s JVM version of its standard library depends on the Java Class Library, hence this tutorial will first show the reader how to install Java SDK and then a Kotlin compiler on [Ubuntu 20.04](https://linuxconfig.org/ubuntu-20-04-guide).

**In this tutorial you will learn:**

- How to install Java SDK
- How to install Kotlin compiler
- How to compile simple Kotlin program
- How to run Kotlin program

![Kotlin on Ubuntu 20.04 Focal Fossa Linux](images/01-how-to-install-kotlin-on-ubuntu-20-04-focal-fossa-linux.png)



Kotlin on Ubuntu 20.04 Focal Fossa Linux

## Software Requirements and Conventions Used



| Category    | Requirements, Conventions or Software Version Used           |
| :---------- | :----------------------------------------------------------- |
| System      | [Installed Ubuntu 20.04](https://linuxconfig.org/how-to-install-ubuntu-20-04-focal-fossa-desktop) or [upgraded Ubuntu 20.04 Focal Fossa](https://linuxconfig.org/how-to-upgrade-ubuntu-to-20-04-lts-focal-fossa) |
| Software    | Kotlin Compiler, OpenJDK java                                |
| Other       | Privileged access to your Linux system as root or via the `sudo` command. |
| Conventions | **#** – requires given [linux commands](https://linuxconfig.org/linux-commands) to be executed with root privileges either directly as a root user or by use of `sudo` command **$** – requires given [linux commands](https://linuxconfig.org/linux-commands) to be executed as a regular non-privileged user |

## How to install Kotlin on Ubuntu 20.04 step by step instructions



1. Install desired Java version

    . For example in this case we will go with Java

     

    ```
    openjdk-11-jdk
    ```

    ```
    $ sudo apt install openjdk-11-jdk
    ```
    
2. Next step is to install Kotlin:

    ```
    $ sudo snap install --classic kotlin
    ```

3. Use any text editor and create a file called

     

    ```
    hello.kt
    ```

     

    with the following content:

    ```kotlin
    fun main() {
        println("Hello World!")
    }
    ```

    Compile the Kotlin source code:

    ```
    $ kotlinc hello.kt -include-runtime -d hello.jar
    ```

4.  

    Run the actual Kotlin program:

    

    ```
    $ java -jar hello.jar
    Hello World!
    ```

### Related Linux Tutorials:

- [Things to install on Ubuntu 20.04](https://linuxconfig.org/things-to-install-on-ubuntu-20-04)
- [Things to do after installing Ubuntu 20.04 Focal Fossa Linux](https://linuxconfig.org/things-to-do-after-installing-ubuntu-20-04-focal-fossa-linux)
- [Ubuntu 20.04 Tricks and Things you Might not Know](https://linuxconfig.org/ubuntu-20-04-tricks-and-things-you-might-not-know)
- [How to Install Adobe Acrobat Reader DC (WINE) on…](https://linuxconfig.org/how-to-install-adobe-acrobat-reader-dc-wine-on-ubuntu-20-04-focal-fossa-linux)
- [How to Install Adobe Acrobat Reader on Ubuntu 20.04…](https://linuxconfig.org/how-to-install-adobe-acrobat-reader-on-ubuntu-20-04-focal-fossa-linux)
- [Oracle Java installation on Ubuntu 20.04 Focal Fossa Linux](https://linuxconfig.org/oracle-java-installation-on-ubuntu-20-04-focal-fossa-linux)
- [Ubuntu 20.04 Hadoop](https://linuxconfig.org/ubuntu-20-04-hadoop)
- [How to Install Java on RHEL 8 / CentOS 8 Linux](https://linuxconfig.org/how-to-install-java-on-redhat-8-linux)
- [List of LaTeX editors and compilers on Ubuntu 20.04…](https://linuxconfig.org/list-of-latex-editors-and-compilers-on-ubuntu-20-04-focal-fossa-linux)
- [How to work with the Woocommerce REST API with Python](https://linuxconfig.org/how-to-work-with-the-woocommerce-rest-api-with-python)