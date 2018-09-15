# gradle init --type java-application


```sh
gradle init --type'basic', 'groovy-library', 'java-application', 'java-library', 'pom', 'scala-library'
```

```sh
gradle init --type java-application
```

vim src/main/java/Main.java

```java
public class Main
{
    public static final void main(String[] args)
    {
        System.out.println("test File");
    }
}
```

##編譯

```sh
gradle build
```
##執行
```sh
java -cp build/classes/main/ Main
```