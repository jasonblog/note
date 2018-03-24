# java命令行編譯運行多個java文件


##一.環境：

1. linux

2. 有兩個java類，A.java和B.java，都在同一個目錄下

##  源碼：


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

## 三.編譯運行

```sh
#編譯  
javac -d . A.java B.java  
  
#運行  
java B  
  
#輸出  
A:test()  
```
