# java中如何给Runnable线程传递参数？

向线程中传递数据的三种方法：
##一、通过构造函数传递参数

```java
public class MyThread1 extends Thread
{
    private String name;
    public MyThread1(String name)
    {
        this.name = name;
    }
    public void run()
    {
        System.out.println("hello " + name);
    }
    public static void main(String[] args)
    {
        Thread thread = new MyThread1("world");
        thread.start();        
    }
}
```

## 二、通过变量和方法传递数据

```java
public class MyThread2 implements Runnable
{
    private String name;
    public void setName(String name)
    {
        this.name = name;
    }
    public void run()
    {
        System.out.println("hello " + name);
    }
    public static void main(String[] args)
    {
        MyThread2 myThread = new MyThread2();
        myThread.setName("world");
        Thread thread = new Thread(myThread);
        thread.start();
    }
}
```

##三、通过回调函数传递数据

```java
class Data
{
    public int value = 0;
}
class Work
{
    public void process(Data data, Integer numbers)
    {
        for (int n : numbers)
        {
            data.value += n;
        }
    }
}
public class MyThread3 extends Thread
{
    private Work work;
 
    public MyThread3(Work work)
    {
        this.work = work;
    }
    public void run()
    {
        java.util.Random random = new java.util.Random();
        Data data = new Data();
        int n1 = random.nextInt(1000);
        int n2 = random.nextInt(2000);
        int n3 = random.nextInt(3000);
        work.process(data, n1, n2, n3);   // 使用回调函数
        System.out.println(String.valueOf(n1) + "+" + String.valueOf(n2) + "+"
                + String.valueOf(n3) + "=" + data.value);
    }
    public static void main(String[] args)
    {
        Thread thread = new MyThread3(new Work());
        thread.start();
    }
}
```