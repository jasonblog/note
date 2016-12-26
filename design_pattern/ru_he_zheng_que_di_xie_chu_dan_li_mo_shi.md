# 如何正確地寫出單例模式


單例模式算是設計模式中最容易理解，也是最容易手寫代碼的模式了吧。但是其中的坑卻不少，所以也常作為面試題來考。本文主要對幾種單例寫法的整理，並分析其優缺點。很多都是一些老生常談的問題，但如果你不知道如何創建一個線程安全的單例，不知道什麼是雙檢鎖，那這篇文章可能會幫助到你。

##懶漢式，線程不安全
當被問到要實現一個單例模式時，很多人的第一反應是寫出如下的代碼，包括教科書上也是這樣教我們的。

```java
public class Singleton {
    private static Singleton instance;
    private Singleton (){}

    public static Singleton getInstance() {
     if (instance == null) {
         instance = new Singleton();
     }
     return instance;
    }
}
```

這段代碼簡單明了，而且使用了懶加載模式，但是卻存在致命的問題。當有多個線程並行調用 getInstance() 的時候，就會創建多個實例。也就是說在多線程下不能正常工作。

##懶漢式，線程安全
為瞭解決上面的問題，最簡單的方法是將整個 getInstance() 方法設為同步（synchronized）。

```java
public static synchronized Singleton getInstance() {
    if (instance == null) {
        instance = new Singleton();
    }
    return instance;
}
```
雖然做到了線程安全，並且解決了多實例的問題，但是它並不高效。因為在任何時候只能有一個線程調用 getInstance() 方法。但是同步操作只需要在第一次調用時才被需要，即第一次創建單例實例對象時。這就引出了雙重檢驗鎖。

##雙重檢驗鎖
雙重檢驗鎖模式（double checked locking pattern），是一種使用同步塊加鎖的方法。程序員稱其為雙重檢查鎖，因為會有兩次檢查 instance == null，一次是在同步塊外，一次是在同步塊內。為什麼在同步塊內還要再檢驗一次？因為可能會有多個線程一起進入同步塊外的 if，如果在同步塊內不進行二次檢驗的話就會生成多個實例了。

```java
public static Singleton getSingleton() {
    if (instance == null) {                         //Single Checked
        synchronized (Singleton.class) {
            if (instance == null) {                 //Double Checked
                instance = new Singleton();
            }
        }
    }
    return instance ;
}
```

這段代碼看起來很完美，很可惜，它是有問題。主要在於instance = new Singleton()這句，這並非是一個原子操作，事實上在 JVM 中這句話大概做了下面 3 件事情。

- 給 instance 分配內存
- 調用 Singleton 的構造函數來初始化成員變量
- 將instance對象指向分配的內存空間（執行完這步 instance 就為非 null 了）


但是在 JVM 的即時編譯器中存在指令重排序的優化。也就是說上面的第二步和第三步的順序是不能保證的，最終的執行順序可能是 1-2-3 也可能是 1-3-2。如果是後者，則在 3 執行完畢、2 未執行之前，被線程二搶佔了，這時 instance 已經是非 null 了（但卻沒有初始化），所以線程二會直接返回 instance，然後使用，然後順理成章地報錯。

我們只需要將 instance 變量聲明成 volatile 就可以了。

```java
public class Singleton {
    private volatile static Singleton instance; //聲明成 volatile
    private Singleton (){}

    public static Singleton getSingleton() {
        if (instance == null) {                         
            synchronized (Singleton.class) {
                if (instance == null) {       
                    instance = new Singleton();
                }
            }
        }
        return instance;
    }
   
}
```
有些人認為使用 volatile 的原因是可見性，也就是可以保證線程在本地不會存有 instance 的副本，每次都是去主內存中讀取。但其實是不對的。使用 volatile 的主要原因是其另一個特性：禁止指令重排序優化。也就是說，在 volatile 變量的賦值操作後面會有一個內存屏障（生成的彙編代碼上），讀操作不會被重排序到內存屏障之前。比如上面的例子，取操作必須在執行完 1-2-3 之後或者 1-3-2 之後，不存在執行到 1-3 然後取到值的情況。從「先行發生原則」的角度理解的話，就是對於一個 volatile 變量的寫操作都先行發生於後面對這個變量的讀操作（這裡的「後面」是時間上的先後順序）。

但是特別注意在 Java 5 以前的版本使用了 volatile 的雙檢鎖還是有問題的。其原因是 Java 5 以前的 JMM （Java 內存模型）是存在缺陷的，即時將變量聲明成 volatile 也不能完全避免重排序，主要是 volatile 變量前後的代碼仍然存在重排序問題。這個 volatile 屏蔽重排序的問題在 Java 5 中才得以修復，所以在這之後才可以放心使用 volatile。

相信你不會喜歡這種複雜又隱含問題的方式，當然我們有更好的實現線程安全的單例模式的辦法。

##餓漢式 static final field
這種方法非常簡單，因為單例的實例被聲明成 static 和 final 變量了，在第一次加載類到內存中時就會初始化，所以創建實例本身是線程安全的。
```java
public class Singleton{
    //類加載時就初始化
    private static final Singleton instance = new Singleton();
    
    private Singleton(){}

    public static Singleton getInstance(){
        return instance;
    }
}
```

這種寫法如果完美的話，就沒必要在囉嗦那麼多雙檢鎖的問題了。缺點是它不是一種懶加載模式（lazy initialization），單例會在加載類後一開始就被初始化，即使客戶端沒有調用 getInstance()方法。餓漢式的創建方式在一些場景中將無法使用：譬如 Singleton 實例的創建是依賴參數或者配置文件的，在 getInstance() 之前必須調用某個方法設置參數給它，那樣這種單例寫法就無法使用了。

##靜態內部類 static nested class
我比較傾向於使用靜態內部類的方法，這種方法也是《Effective Java》上所推薦的。

```java
public class Singleton {  
    private static class SingletonHolder {  
        private static final Singleton INSTANCE = new Singleton();  
    }  
    private Singleton (){}  
    public static final Singleton getInstance() {  
        return SingletonHolder.INSTANCE; 
    }  
}
```

這種寫法仍然使用JVM本身機制保證了線程安全問題；由於 SingletonHolder 是私有的，除了 getInstance() 之外沒有辦法訪問它，因此它是懶漢式的；同時讀取實例的時候不會進行同步，沒有性能缺陷；也不依賴 JDK 版本。

##枚舉 Enum
用枚舉寫單例實在太簡單了！這也是它最大的優點。下面這段代碼就是聲明枚舉實例的通常做法。

```java
public enum EasySingleton{
    INSTANCE;
}
```

我們可以通過EasySingleton.INSTANCE來訪問實例，這比調用getInstance()方法簡單多了。創建枚舉默認就是線程安全的，所以不需要擔心double checked locking，而且還能防止反序列化導致重新創建新的對象。但是還是很少看到有人這樣寫，可能是因為不太熟悉吧。

##總結
一般來說，單例模式有五種寫法：懶漢、餓漢、雙重檢驗鎖、靜態內部類、枚舉。上述所說都是線程安全的實現，文章開頭給出的第一種方法不算正確的寫法。

就我個人而言，一般情況下直接使用餓漢式就好了，如果明確要求要懶加載（lazy initialization）會傾向於使用靜態內部類，如果涉及到反序列化創建對象時會試著使用枚舉的方式來實現單例。

##Read More
Double Checked Locking on Singleton Class in Java
Why Enum Singleton are better in Java
How to create thread safe Singleton in Java
10 Singleton Pattern Interview questions in Java