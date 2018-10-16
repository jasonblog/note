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
