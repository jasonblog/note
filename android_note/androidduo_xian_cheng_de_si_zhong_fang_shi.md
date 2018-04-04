# Android多线程的四种方式


当我们启动一个App的时候，Android系统会启动一个`Linux Process，该Process包含一个Thread，称为UI Thread或Main Thread。`通常一个应用的所有组件都运行在这一个Process中，当然，你可以通过修改四大组件在Manifest.xml中的代码块`(<activity><service><provider><receiver>)`中的android:process属性指定其运行在不同的process中。当一个组件在启动的时候，如果该process已经存在了，那么该组件就直接通过这个process被启动起来，并且运行在这个process的UI Thread中。

UI Thread中运行着许多重要的逻辑，如系统事件处理，用户输入事件处理，UI绘制，Service，Alarm等，如下图：

![](images/2839011-23a52dcb6f73550f.png)

而我们编写的代码则是穿插在这些逻辑中间，比如对用户触摸事件的检测和响应，对用户输入的处理，自定义View的绘制等。如果我们插入的代码比价耗时，如网络请求或数据库读取，就会阻塞UI线程其他逻辑的执行，从而导致界面卡顿。如果卡顿时间超过5秒，系统就会报ANR错误。所以，如果要执行耗时的操作，我们需要另起线程执行。

在新线程执行完耗时的逻辑后，往往需要将结果反馈给界面，进行UI更新。Android的UI toolkit不是线程安全的，不能在非UI线程进行UI的更新，所有对界面的更新必须在UI线程进行。

Android提供了四种常用的操作多线程的方式，分别是：

```sh
1. Handler+Thread
2. AsyncTask
3. ThreadPoolExecutor
4. IntentService
```

下面分布对四种方式进行介绍。

##Handler+Thread
Android主线程包含一个消息队列(MessageQueue)，该消息队列里面可以存入一系列的Message或Runnable对象。通过一个Handler你可以往这个消息队列发送Message或者Runnable对象，并且处理这些对象。每次你新创建一个Handle对象，它会绑定于创建它的线程(也就是UI线程)以及该线程的消息队列，从这时起，这个handler就会开始把Message或Runnable对象传递到消息队列中，并在它们出队列的时候执行它们。

![](images/2839011-7036df5ffea97ec2.png)


Handler可以把一个Message对象或者Runnable对象压入到消息队列中，进而在UI线程中获取Message或者执行Runnable对象，Handler把压入消息队列有两类方式，Post和sendMessage：

Post方式：
Post允许把一个Runnable对象入队到消息队列中。它的方法有：
post(Runnable)/postAtTime(Runnable,long)/postDelayed(Runnable,long)

对于Handler的Post方式来说，它会传递一个Runnable对象到消息队列中，在这个Runnable对象中，重写run()方法。一般在这个run()方法中写入需要在UI线程上的操作。


![](images/2839011-b1d72fd4b937a29e.png)

##sendMessage：
sendMessage允许把一个包含消息数据的Message对象压入到消息队列中。它的方法有：sendEmptyMessage(int)/sendMessage(Message)/sendMessageAtTime(Message,long)/sendMessageDelayed(Message,long)

Handler如果使用sendMessage的方式把消息入队到消息队列中，需要传递一个Message对象，而在Handler中，需要重写handleMessage()方法，用于获取工作线程传递过来的消息，此方法运行在UI线程上。Message是一个final类，所以不可被继承。


![](images/2839011-cdfe976cd627891b.png)
![](images/2839011-2dd38fe2116901a0.png)


##优缺点

```sh
1. Handler用法简单明了，可以将多个异步任务更新UI的代码放在一起，清晰明了
2. 处理单个异步任务代码略显多
```

##适用范围

```sh
1. 多个异步任务的更新UI
```

##AsyncTask

AsyncTask是android提供的轻量级的异步类,可以直接继承AsyncTask，在类中实现异步操作，并提供接口反馈当前异步执行的程度(可以通过接口实现UI进度更新)，最后反馈执行的结果给UI主线程。

AsyncTask通过一个阻塞队列BlockingQuery<Runnable>存储待执行的任务，利用静态线程池THREAD_POOL_EXECUTOR提供一定数量的线程，默认128个。在Android 3.0以前，默认采取的是并行任务执行器，3.0以后改成了默认采用串行任务执行器，通过静态串行任务执行器SERIAL_EXECUTOR控制任务串行执行，循环取出任务交给THREAD_POOL_EXECUTOR中的线程执行，执行完一个，再执行下一个。

用法举例：


```java
class DownloadTask extends AsyncTask<Integer, Integer, String> {
    // AsyncTask<Params, Progress, Result>
    //后面尖括号内分别是参数（例子里是线程休息时间），进度(publishProgress用到)，返回值类型
    @Override
    protected void onPreExecute() {
        //第一个执行方法
        super.onPreExecute();
    }
    @Override
    protected String doInBackground(Integer... params) {
        //第二个执行方法,onPreExecute()执行完后执行
        for(int i=0; i<=100; i++) {
            publishProgress(i);
            try {
                Thread.sleep(params[0]);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        return "执行完毕";
    }
    @Override
    protected void onProgressUpdate(Integer... progress) {
        //这个函数在doInBackground调用publishProgress时触发，虽然调用时只有一个参数
        //但是这里取到的是一个数组,所以要用progesss[0]来取值
        //第n个参数就用progress[n]来取值
        tv.setText(progress[0]+"%");
        super.onProgressUpdate(progress);
    }
    @Override
    protected void onPostExecute(String result) {
        //doInBackground返回时触发，换句话说，就是doInBackground执行完后触发
        //这里的result就是上面doInBackground执行后的返回值，所以这里是"执行完毕"
        setTitle(result);
        super.onPostExecute(result);
    }
}
```

###优缺点
```sh
1. 处理单个异步任务简单，可以获取到异步任务的进度
2. 可以通过cancel方法取消还没执行完的AsyncTask
3. 处理多个异步任务代码显得较多
```

###适用范围
```sh
1. 单个异步任务的处理
```
##ThreadPoolExecutor

ThreadPoolExecutor提供了一组线程池，可以管理多个线程并行执行。这样一方面减少了每个并行任务独自建立线程的开销，另一方面可以管理多个并发线程的公共资源，从而提高了多线程的效率。所以ThreadPoolExecutor比较适合一组任务的执行。Executors利用工厂模式对ThreadPoolExecutor进行了封装，使用起来更加方便。

![](images/2839011-d3869fae4dff8272.png)

Executors提供了四种创建ExecutorService的方法，他们的使用场景如下：

```sh
1. Executors.newFixedThreadPool()
   创建一个定长的线程池，每提交一个任务就创建一个线程，直到达到池的最大长度，这时线程池会保持长度不再变化
2. Executors.newCachedThreadPool()
   创建一个可缓存的线程池，如果当前线程池的长度超过了处理的需要时，它可以灵活的回收空闲的线程，当需要增加时，
    它可以灵活的添加新的线程，而不会对池的长度作任何限制
3. Executors.newScheduledThreadPool()
   创建一个定长的线程池，而且支持定时的以及周期性的任务执行，类似于Timer
4. Executors.newSingleThreadExecutor()
   创建一个单线程化的executor，它只创建唯一的worker线程来执行任务
```

##适用范围
```sh
1. 批处理任务
```

## IntentService
IntentService继承自Service，是一个经过包装的轻量级的Service，用来接收并处理通过Intent传递的异步请求。客户端通过调用startService(Intent)启动一个IntentService，利用一个work线程依次处理顺序过来的请求，处理完成后自动结束Service。

##特点
```sh
1. 一个可以处理异步任务的简单Service
```