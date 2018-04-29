# Java -jar 選項與 -cp/-classpath


今天遇到一個問題，我雖然把所有的引用到的jar文件都放到了 -cp選項裡，但是我仍然無法運行jar文件: 
  
```sh
jar -cp lib/referenced.jar -jar myworks.jar  
```

後來到網上查閱了一下，發現如果我們使用-jar選項的話java.exe`會忽略-cp`,-classpath，以及環境變量CLASSPATH的參數。 

解決方法如下： 
1>不要使用-jar選項，直接調用含有main方法的class文件，這樣-cp,-classpath以及環境變量裡的CLASSPATH指定的參數就都能使用到了。 
  

```sh
java -classpath ./lib/junit.jar:. test/Test1  
```

2>繼續使用-jar選項,但是在MAINFEST.MF文件中指定引用到jar文件. 
  

```sh
Class-Path: myplace/myjar.jar myplace/other.jar jardir/  
```
   
另外說明一點，這個問題可能有些人遇不到，因為Java的版本不同的原因，我在Sun的JDK和IBM 1.5的JDK都遇到了這個問題，但是對於 IBM 1.4的JDK卻沒有類似問題。


---


```sh
javac -d out/ ../../../src/c0/singleton/*.java SingletonTest.java
java -cp ./out/ c0.singleton.SingletonTest
```

```sh
javac -d out/ -cp /usr/share/java/junit4-4.12.jar ../../../src/c01/simpleFactory/village/*.java *.java
```


---

## 使用junit测试java程序方法

java -classpath .:/usr/share/java/junit4-4.12.jar org.junit.runner.JUnitCore HelloWorldTest


```java
// HelloWorld.java
import java.util.*;
public final class HelloWorld
{
    private String str;
    public static void main(String[] args)
    {
        HelloWorld  hw = new HelloWorld();
        hw.hello();
        System.out.println(hw.str);
    }
    public void hello()
    {
        str = "Hello World!";
    }
    public String getStr()
    {
        return str;
    }
}
```


```java
// HelloWorldTes.java
import static org.junit.Assert.*;
import org.junit.Test;
public class HelloWorldTest
{
    public HelloWorld helloworld = new HelloWorld();
    @Test
    public void testHello()
    {
        helloworld.hello();
        assertEquals("Hello World!",  helloworld.getStr());
    }
}
```

```sh
javac -d . -cp /usr/share/java/junit4-4.12.jar *.java

java -classpath .:/usr/share/java/junit4-4.12.jar org.junit.runner.JUnitCore HelloWorldTest
```



