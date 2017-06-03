# Thread和Runnable的区别


多线程 Thread Runnable 
大家都知道多线程可以通过两种方式来创建。

一、通过继承Thread类

二、通过实现Runnable接口

那么中两种方式到底有什么区别呢？那种方式更好些呢？

我们使用多线程，无非就是想利用多线程的优点，来更好的实现我们的想法。

多线程有哪些优点呢？

一、多线程共享同一块内存空间和一组系统资源

二、线程本身的数据通常都是只有微处理器的寄存器数据，以及供程序执行的堆栈。所以系统在产生一个线程或者线程的切换要比进程系统的负担小很多。

现在我们来了解一些Thread类。

线程代码

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

运行后，我们发现其实只启动了一个线程。

是不是程序有问题呢？我们修改一下程序。


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

 再次运行，发现这`四个线程对象各自占有各自的资源`，并不是同时完成统一任务。

我们可以得出结论：`Thread类实际上无法达到资源共享的目的`。

那么，Runnable接口能不能达到这一目的呢？

我们大可试一下。

Runnable代码

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

运行之后我们发现，这四个线程同时完成了我们需要完成的任务。

通过以上比较我们即可得出Thread与Runnable的区别：

1、Runnable适合于多个相同程序代码线程去处理统一资源的情况，把虚拟的cpu（线程）同程序的代码，数据有效分离，较好体现面向对象的编程的思想

2、Runnable可以避免由于java的单继承机制带来的局限。可以再继承其他类的同时，还能实现多线程的功能。

3、Runnable能增加程序的健壮性。代码能够被多个线程共享。


