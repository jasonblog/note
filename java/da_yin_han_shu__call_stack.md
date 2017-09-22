# 打印函數 / call stack


```java
System.out.println((new Throwable()).getStackTrace()[0].toString());
```


結果:
com.ecs.ces.ruleengine.TestSocket.parserpackage(TestSocket.java:72)


```java
import java.util.Map;

for (Map.Entry <Thread, StackTraceElement []> entry :
     Thread.getAllStackTraces().entrySet())
{
    System.out.println(entry.getKey().getName() + ":");

    for (StackTraceElement element : entry.getValue()) {
        System.out.println("\t" + element);
    }
}
```
