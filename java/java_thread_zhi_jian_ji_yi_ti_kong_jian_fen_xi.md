# Java Thread 之間 記憶體空間分析


- SimpleThread.java

```java
import com.abc.ChirpControllerDevice;

public class SimpleThread
{
    public static void main(String[] args)
    {
        ChirpControllerDevice mC0 = null;

        Thread thread = new Thread(new Runnable() {
            public void run() {
                while (true) {
                    try {
                        Thread.sleep(5000);
                    } catch (InterruptedException e) {
                        System.out.println(e);
                    }

                    System.out.println("T");
                }

            }
        });
        thread.start();

        mC0 = new ChirpControllerDevice();
        mC0.init();

        /*
        while (true) {
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                System.out.println(e);
            }
        }
        */

        // 主執行緒繼續進行其它工作........
        // such...such....
        // 現在主執行緒執行到這邊了，工作應該結束了
    }
}

```

- ChirpControllerDevice.java

```java
package com.abc;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.lang.Math;

public class ChirpControllerDevice
{
    private String TAG = "";
    private boolean DEBUG = true;
    private Object mSimulatorlocker = new Object();
    private Thread mPollingThread;
    private int a;

    public ChirpControllerDevice()
    {
    }

    public void init()
    {
        mPollingThread = new Thread(new ChirpPollingRunnable());
        mPollingThread.start();
    }

    public class ChirpPollingRunnable implements Runnable
    {
        public ChirpPollingRunnable()
        {
        }

        @Override
        public void run()
        {
            while (true) {
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    System.out.println(e);
                }

                System.out.println("a" + a);

            }
        }
    }
}
```


```sh
javac -d . ChirpControllerDevice.java SimpleThread.java
java SimpleThread
```