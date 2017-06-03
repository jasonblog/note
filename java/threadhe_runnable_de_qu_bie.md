# Thread和Runnable的區別


多線程 Thread Runnable 
大家都知道多線程可以通過兩種方式來創建。

一、通過繼承Thread類

二、通過實現Runnable接口

那麼中兩種方式到底有什麼區別呢？那種方式更好些呢？

我們使用多線程，無非就是想利用多線程的優點，來更好的實現我們的想法。

多線程有哪些優點呢？

一、多線程共享同一塊內存空間和一組系統資源

二、線程本身的數據通常都是隻有微處理器的寄存器數據，以及供程序執行的堆棧。所以系統在產生一個線程或者線程的切換要比進程系統的負擔小很多。

現在我們來了解一些Thread類。

線程代碼

```java
public class ThreadDemo
{
    public static void main(String []args)
    {
        ThreadTest t = new ThreadTest();
        t.start();
        t.start();
        t.start();
        t.start();
    }
}

class ThreadTest extends Thread
{
    private int count = 10;
    public void run()
    {
        while (count > 0) {
            System.out.println(Thread.currentThread().getName() + "   " + count--);
        }
    }
}
```

運行後，我們發現其實只啟動了一個線程。

是不是程序有問題呢？我們修改一下程序。


```java
public  class ThreadDemo
{
    public static void main(String []args)
    {
        new ThreadTest().start();
        new ThreadTest().start();
        new ThreadTest().start();
        new ThreadTest().start();
    }
}

class ThreadTest extends Thread
{
    private int count = 10;
    public void run()
    {
        while (count > 0) {
            System.out.println(Thread.currentThread().getName() + "   " + count--);
        }
    }
}
```

 再次運行，發現這`四個線程對象各自佔有各自的資源`，並不是同時完成統一任務。

我們可以得出結論：`Thread類實際上無法達到資源共享的目的`。

那麼，Runnable接口能不能達到這一目的呢？

我們大可試一下。

Runnable代碼

```java
public  class ThreadDemo
{
    public static void main(String []args)
    {
        ThreadTest test = new ThreadTest();
        new Thread(test).start();
        new Thread(test).start();
        new Thread(test).start();
        new Thread(test).start();
    }
}

class ThreadTest implements Runnable
{
    private int count = 10;
    public void run()
    {
        while (count > 0) {
            System.out.println(Thread.currentThread().getName() + "   " + count--);
        }
    }
}
```

運行之後我們發現，這四個線程同時完成了我們需要完成的任務。

通過以上比較我們即可得出Thread與Runnable的區別：

1、Runnable適合於多個相同程序代碼線程去處理統一資源的情況，把虛擬的cpu（線程）同程序的代碼，數據有效分離，較好體現面向對象的編程的思想

2、Runnable可以避免由於java的單繼承機制帶來的侷限。可以再繼承其他類的同時，還能實現多線程的功能。

3、Runnable能增加程序的健壯性。代碼能夠被多個線程共享。


