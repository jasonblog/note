# java命令行编译运行多个java文件


##一.环境：

1. linux

2. 有两个java类，A.java和B.java，都在同一个目录下

##  源码：


- A.java

```java
package com.abc;  
public class A {  
    public static void test() {  
        System.out.println("A:test()");  
    }  
}  
```

- B.java

```java
import com.abc.A;  
public class B {  
    public static void main(String[] argc) {  
        A a = new A();  
        a.test();  
    }  
}
```

## 三.编译运行

```sh
#编译  
javac -d . A.java B.java  
  
#运行  
java B  
  
#输出  
A:test()  
```
