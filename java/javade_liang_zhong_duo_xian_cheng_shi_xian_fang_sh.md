# Java的两种多线程实现方式


##一、创建多线程的两种方式
Java中，有两种方式可以创建多线程：

1 通过继承Thread类，重写Thread的run()方法，将线程运行的逻辑放在其中

2 通过实现Runnable接口，实例化Thread类
在实际应用中，我们经常用到多线程，如车站的售票系统，车站的各个售票口相当于各个线程。当我们做这个系统的时候可能会想到两种方式来实现，继承Thread类或实现Runnable接口，现在看一下这两种方式实现的两种结果。

程序1：

```java
class MyThread extends Thread
{
    private int ticket = 10;
    private String name;
    public  MyThread(String name)
    {
        this.name = name;
    }
    public void run()
    {
        for (int i = 0; i < 500; i++) {
            if (this.ticket > 0) {
                System.out.println(this.name + "卖票---->" + (this.ticket--));
            }
        }
    }
}

public class ThreadDemo
{
    public static void main(String[] args)
    {
        MyThread mt1 = new MyThread("一号窗口");
        MyThread mt2 = new MyThread("二号窗口");
        MyThread mt3 = new MyThread("三号窗口");
        mt1.start();
        mt2.start();
        mt3.start();
    }
}
```
运行结果：
```sh
一号窗口卖票---->10
三号窗口卖票---->10
二号窗口卖票---->10
三号窗口卖票---->9
三号窗口卖票---->8
三号窗口卖票---->7
一号窗口卖票---->9
三号窗口卖票---->6
二号窗口卖票---->9
三号窗口卖票---->5
一号窗口卖票---->8
三号窗口卖票---->4
二号窗口卖票---->8
三号窗口卖票---->3
一号窗口卖票---->7
三号窗口卖票---->2
二号窗口卖票---->7
三号窗口卖票---->1
一号窗口卖票---->6
一号窗口卖票---->5
一号窗口卖票---->4
一号窗口卖票---->3
二号窗口卖票---->6
一号窗口卖票---->2
二号窗口卖票---->5
一号窗口卖票---->1
二号窗口卖票---->4
二号窗口卖票---->3
二号窗口卖票---->2
```

程序2：

```java
class MyThread1 implements Runnable
{
    private int ticket = 10;
    public void run()
    {
        for (int i = 0; i < 500; i++) {
            if (this.ticket > 0) {
                System.out.println(Thread.currentThread().getName()
                                      + "卖票---->" + (this.ticket--));
            }
        }
    }
}

public class RunnableDemo {
    public static void main(String[] args)
    {
        // 设计三个线程
        MyThread1 mt = new MyThread1();
        Thread t1 = new Thread(mt, "一号窗口");
        Thread t2 = new Thread(mt, "二号窗口");
        Thread t3 = new Thread(mt, "三号窗口");
        t1.start();
        t2.start();
        t3.start();
    }
}
```
运行结果：

```sh
一号窗口卖票---->10
二号窗口卖票---->8
二号窗口卖票---->6
二号窗口卖票---->5
三号窗口卖票---->9
二号窗口卖票---->4
一号窗口卖票---->7
二号窗口卖票---->2
三号窗口卖票---->3
一号窗口卖票---->1
```


为什么两个程序的结果不同呢？

第1个程序，相当于拿出三件事即三个卖票10张的任务分别分给三个窗口，他们各做各的事各卖各的票各完成各的任务，因为MyThread继承Thread类，所以在new
MyThread的时候在创建三个对象的同时创建了三个线程。

第2个程序，相当于是拿出一个卖票10张得任务给三个人去共同完成，new
MyThread相当于创建一个任务，然后实例化三个Thread，创建三个线程即安排三个窗口去执行。

用图表示如下：


![](images/6946981-d907c4ffbd7c1ba9.jpg)

通过上面的分析，我们发现这两种多线程有两大区别：

(1)
Thread方式是继承；Runnable方式是实现接口。

(2)
Thread方式是多个线程分别完成自己的任务，即数据独立；Runnable方式是多个线程共同完成一个任务，即数据共享。

大多数情况下，如果只想重写run() 方法，而不重写其他 Thread 方法，那么应使用 Runnable 接口。这很重要，因为除非程序员打算修改或增强类的基本行为，否则不应为该类（Thread）创建子类。

##二、隐藏的问题


在第二种方法中，由于3个Thread对象共同执行一个Runnable对象中的代码，因此可能会造成线程的不安全，比如可能ticket会输出-1（如果我们System.out....语句前加上线程休眠操作，该情况将很有可能出现）。

这种情况的出现是由于，一个线程在判断ticket为1>0后，还没有来得及减1，另一个线程已经将ticket减1，变为了0，那么接下来之前的线程再将ticket减1，便得到了-1。

这就需要加入同步操作（即互斥锁），确保同一时刻只有一个线程在执行每次for循环中的操作。

而在第一种方法中，并不需要加入同步操作，因为每个线程执行自己Thread对象中的代码，不存在多个线程共同执行同一个方法的情况。


程序1：

```sh
class MyThread1 implements Runnable
{
    private int ticket = 10;
    public void run()
    {
        for (int i = 0; i < 500; i++) {
            if (this.ticket > 0) {
                try {
                    Thread.sleep(100);
                    System.out.println(Thread.currentThread().getName()
                                          + "卖票---->" + (this.ticket--));
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }
}

public class RunnableDemo {
    public static void main(String[] args)
    {
        // 设计三个线程
        MyThread1 mt = new MyThread1();
        Thread t1 = new Thread(mt, "一号窗口");
        Thread t2 = new Thread(mt, "二号窗口");
        Thread t3 = new Thread(mt, "三号窗口");
        t1.start();
        t2.start();
        t3.start();
    }
}
```

运行结果：

```sh
三号窗口卖票---->10
一号窗口卖票---->10
二号窗口卖票---->10
三号窗口卖票---->9
一号窗口卖票---->8
二号窗口卖票---->7
三号窗口卖票---->6
一号窗口卖票---->5
二号窗口卖票---->4
三号窗口卖票---->3
一号窗口卖票---->2
二号窗口卖票---->1
三号窗口卖票---->0
```

注意，这里的10张票都是一号窗口卖出的。这是因为用了synchronized并且票数太少了，在t1对this对象锁定的时间内，10张票就已经被卖完了。轮到t2或t3锁定this对象时，已经无票可卖了。如果票数多一点，比如有几万张，就可以看到三个窗口都参与了卖票。



程序2：


```java
class MyThread1 implements Runnable
{
    private int ticket = 1000;
    public void run()
    {
        for (int i = 0; i < 5000; i++) {
            synchronized(this) {
                if (this.ticket > 0) {
                    System.out.println(Thread.currentThread().getName() + "卖票---->" +
                                       (this.ticket--));
                }
            }
        }
    }
}
public class RunnableDemo {
    public static void main(String[] args)
    {
        // 设计三个线程
        MyThread1 mt = new MyThread1();
        Thread t1 = new Thread(mt, "一号窗口");
        Thread t2 = new Thread(mt, "二号窗口");
        Thread t3 = new Thread(mt, "三号窗口");
        t1.start();
        t2.start();
        t3.start();
    }
}
```

