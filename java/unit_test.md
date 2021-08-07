## Unit Test

- Junit  test on ubuntu 

```java
// HelloWorld.java
import java.util.*;

public final class HelloWorld {
    private String str;
    public static void main(String[] args) {
        HelloWorld  hw = new HelloWorld();
        hw.hello();
        System.out.println(hw.str);
    }
    public void hello() {
        str = "Hello World!";
    }
    public String getStr() {
        return str;
    }
}
```

```java
// HelloWorldTesT.java
import static org.junit.Assert.*;
import org.junit.Test;

public class HelloWorldTest {
    public HelloWorld helloworld = new HelloWorld();
    @Test
    public void testHello() {
        helloworld.hello();
        assertEquals("Hello World!",  helloworld.getStr());
    }
}
```

```bash
sudo apt-get install junit4

javac HelloWorld.java
javac HelloWorldTest.java -cp ./:/usr/share/java/junit4.jar
java -cp .:/usr/share/java/junit4.jar org.junit.runner.JUnitCore HelloWorldTest
```



## Android Unit Test

```bash
git clone https://github.com/shihyu/UnitTestDemo

android studio 編譯 & Run
Run ExampleUnitTest
```

