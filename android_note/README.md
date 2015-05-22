# Service与Thread的区别


- 五种交互方式，分别是：通过广播交互、通过共享文件交互、通过Messenger(信使)交互、通过自定义接口交互、通过AIDL交互。（可能更多）


### Service与Thread的区别
Thread：Thread 是程序执行的最小单元，可以用 Thread 来执行一些异步的操作。

Service：Service 是android的一种机制，当它运行的时候如果是Local Service，那么对应的 Service 是运行在主进程的 main 线程上的。如果是Remote Service，那么对应的 Service 则是运行在独立进程的 main 线程上。



Thread 的运行是独立的，也就是说当一个 Activity 被 finish 之后，如果没有主动停止 Thread 或者 Thread 里的 run 方法没有执行完毕的话，Thread 也会一直执行。因此这里会出现一个问题：当 Activity 被 finish 之后，不再持有该 Thread 的引用，也就是不能再控制该Thread。另一方面，没有办法在不同的 Activity 中对同一 Thread 进行控制。

**例如：如果 一个Thread 需要每隔一段时间连接服务器校验数据，该Thread需要在后台一直运行。这时候如果创建该Thread的Activity被结束了而该Thread没有停止，那么将没有办法再控制该Thread，除非kill掉该程序的进程。这时候如果创建并启动一个 Service ，在 Service 里面创建、运行并控制该 Thread，这样便解决了该问题(因为任何 Activity 都可以控制同一个Service，而系统也只会创建一个对应 Service 的实例)。**

因此可以把 Service 想象成一种消息服务，可以在任何有 Context 的地方调用 Context.startService、Context.stopService、Context.bindService、Context.unbindService来控制它，也可以在 Service 里注册 BroadcastReceiver，通过发送 broadcast 来达到控制的目的，这些都是 Thread 做不到的。


### Service的生命周期
1. 被启动的服务(startService())的生命周期。
如果一个Service被某个Activity 调用Context.startService() 方法启动，那么不管是否有Activity使用bindService()绑定或unbindService()解除绑定到该Service，该Service都在后台运行。如果一个Service被多次执行startService()，它的onCreate()方法只会调用一次，也就是说该Service只会创建一个实例，而它的onStartCommand()将会被调用多次(对应调用startService()的次数)。该Service将会一直在后台运行，直到被调用stopService()，或自身的stopSelf方法。当然如果系统资源不足，系统也可能结束服务。

2. 被绑定的服务(bindService())的生命周期。如果一个Service被调用 Context.bindService ()方法绑定启动，不管调用bindService()调用几次，onCreate()方法都只会调用一次，而onStartCommand()方法始终不会被调用，这时会调用onBind()方法。当连接建立之后，Service将会一直运行，除非调用Context.unbindService() 断开连接或者之前调用bindService() 的 Context 不存在了(如该Activity被finish)，系统将会自动停止Service，对应onDestroy()将被调用。

3. 被启动又被绑定的服务的生命周期。如果一个Service又被启动又被绑定，则该Service将会一直在后台运行。调用unbindService()将不会停止Service，而必须调用stopService()或Service的stopSelf()方法来停止服务。

4. 当服务被停止时清除服务。
当一个Service被终止时，Service的onDestroy()方法将会被调用，在这里应当做一些清除工作，如停止在Service中创建并运行的线程等。


### Process的生命周期

当Service运行在低内存的环境时，系统会kill掉一些进程。因此进程的优先级将会狠重要：
1.    如果Service当前正在执行onCreate()、onStartCommand()、onDestroy()方法，那麼此时主进程将会成为前台进程来保证代码可以执行完成而避免被kill。

2.    如果Service已经启动，那么主进程将会比其他可见的进程的重要性低，但比其他看不见的进程高。这裡说的可见指的是对用户来讲，可见的进程优先级永远是最高的，用户至上嘛。但只有少部分进程始终是用户可见的，因此除非系统处於极度低内存的时候，不然 service是不会被kill的。

3.    如果有Client端连到Service，那么Service永远比Client端重要。

4.    Service可以使用startForeground()将Service放到前台状态。这样在低内存时被kill的几率更低，但如果在极低内存的情况下，该Service理论上还是会被kill掉。但这个情况基本不用考虑。
5.

