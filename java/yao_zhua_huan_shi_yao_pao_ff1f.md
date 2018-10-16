# 要抓還是要拋？

## 抓

```java
import java.util.Scanner;
import java.io.FileInputStream;
import java.io.FileNotFoundException;

class FileUtil
{
    public static String readFile(String name)
    {
        StringBuilder builder = new StringBuilder();

        try {
            Scanner scanner = new Scanner(new FileInputStream(name));

            while (scanner.hasNext()) {
                builder.append(scanner.nextLine());
                builder.append('\n');
            }
        } catch (FileNotFoundException ex) {
            ex.printStackTrace();
        }

        return builder.toString();
    }

    public static void main(String args[])
    {
    }

}
```

由於建構FileInputStream時，API設計者聲明方法實作中會拋出FileNotFoundException，根據目前你學到的例外處理語法，於是你捕捉FileNotFoundException並在主控台中顯示錯誤訊息。


## 拋

主控台？等一下！老闆有說這個程式庫會用在文字模式中嗎？如果這個程式庫是用在Web網站上，發生錯誤時顯示在主控台上，Web使用者怎麼會看得到？你開發的是程式庫，例外發生時如何處理，是程式庫使用者才知道，直接在catch中寫死處理例外或輸出錯誤訊息的方式，並不符合需求。

```java
import java.util.Scanner;
import java.io.FileInputStream;
import java.io.FileNotFoundException;

class FileUtil
{
    public static String readFile(String name) throws FileNotFoundException {
        StringBuilder builder = new StringBuilder();
        Scanner scanner = new Scanner(new FileInputStream(name));

        while (scanner.hasNext())
        {
            builder.append(scanner.nextLine());
            builder.append('\n');
        }

        return builder.toString();
    }

    public static void main(String args[])
    {
    }
}
```
操作物件的過程中如果會拋出受檢例外，但目前環境資訊不足以處理例外，所以無法使用try、catch處理時，`可由方法的客戶端依當時呼叫的環境資訊進行處理。為了告訴編譯器這件事實，必須在方法上使用throws宣告此方法會拋出的例外類型或父類型，編譯器才會讓你通過編譯`。


拋出受檢例外，表示你認為呼叫方法的客戶端有能力且應該處理例外，throws宣告部份，`會是API操作介面的一部份，客戶端不用察看原始碼，從API文件上就能直接得知，該方法可能拋出哪些例外`。


實際上在例外發生時，可使用try、catch處理`當時環境可進行的例外處理`，當時環境下`無法決定如何處理的部份，可以拋出由呼叫方法的客戶端處理`。如果想先處理部份事項再拋出，可以如下：



```java
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.Scanner;

public class FileUtil
{
    public static String readFile(String name) throws FileNotFoundException {
        StringBuilder builder = new StringBuilder();

        try
        {
            Scanner scanner = new Scanner(new FileInputStream(name));

            while (scanner.hasNext()) {
                builder.append(scanner.nextLine());
                builder.append('\n');
            }
        } catch (FileNotFoundException ex)
        {
            ex.printStackTrace();
            throw ex;
        }

        return builder.toString();
    }

    public static void main(String args[])
    {
    }
}
```

範例在catch區塊進行完部份錯誤處理之後，可以使用throw（注意不是throws）將例外再拋出，實際上，你可以在任何流程中拋出例外，`不一定要在catch區塊中`，在流程中拋出例外，`就直接跳離原有的流程`，可以拋出`受檢或非受檢例外`，

記得！


如果拋出的是`受檢例外`，表示你認為客戶端有能力且應處理例外，此時必須在`方法上使用throws宣告`，

如果拋出的例外是`非受檢例外`，表示你認為客戶端呼叫方法的時機出錯了，拋出例外是要求客戶端修正這個臭蟲再來呼叫方法，`此時也就不用使用throws宣告`。



```java
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.Scanner;
import java.io.EOFException;

public class FileUtil
{
    public static void doSome(String arg) throws FileNotFoundException, EOFException {
        try
        {
            if ("one".equals(arg)) {
                throw new FileNotFoundException();
            } else {
                throw new EOFException();
            }
        } catch (FileNotFoundException ex)
        {
            ex.printStackTrace();
            throw ex;
        } catch (EOFException ex)
        {
            ex.printStackTrace();
            throw ex;
        }
    }

    public static void main(String args[])
    {
    }
}
```

如果使用繼承時，父類別某個方法宣告throws某些例外，子類別重新定義該方法時可以：

- 不宣告throws任何例外
- 可throws父類別該方法中宣告的某些例外
- 可throws父類別該方法中宣告例外之子類別

但是不可以：

- throws父類別方法中未宣告的其它例外
- throws父類別方法中宣告例外之父類別


