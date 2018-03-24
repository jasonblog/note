# Java Thread 之間 記憶體空間分析


- SimpleThread.java

```java
import com.abc.ChirpControllerDevice;

public class SimpleThread
{
    public static void main(String[] args)
    {
        ChirpControllerDevice mC0 = null;
        ChirpControllerDevice mC1 = null;

        Thread thread = new Thread(new Runnable() {
            public long getPID() {
                String processName =
                    java.lang.management.ManagementFactory.getRuntimeMXBean().getName();
                return Long.parseLong(processName.split("@")[0]);
            }

            public void run() {
                while (true) {
                    try {
                        Thread.sleep(5000);
                    } catch (InterruptedException e) {
                        System.out.println(e);
                    }

                    System.out.println("T" + ", PID:" + getPID() + ", TID:"+ Thread.currentThread().getId());
                }

            }
        });
        thread.start();

        mC0 = new ChirpControllerDevice(88);
        mC0.init();
        mC1 = new ChirpControllerDevice(66);
        mC1.init();

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
    private int mData;

    public ChirpControllerDevice(int data)
    {
        mData = data;
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

        public long getPID()
        {
            String processName =
                java.lang.management.ManagementFactory.getRuntimeMXBean().getName();
            return Long.parseLong(processName.split("@")[0]);
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

                System.out.println("data:" + mData + ", address:" + this + ", PID:" +getPID() + ", TID:"+ Thread.currentThread().getId());

                /*
                System.out.println(Thread.currentThread().getStackTrace()[2].getClassName() +
                                   "[" + Thread.currentThread().getStackTrace()[2].getMethodName() + " | " +
                                   Thread.currentThread().getStackTrace()[2].getFileName() + ":" +
                                   Thread.currentThread().getStackTrace()[2].getLineNumber() + "]");
                                   */
            }
        }
    }
}
```

- Makefile

```sh
JFLAGS = -g -cp ./classes/ -d ./classes
JC = javac
JVM= java
CLASSESDIR = classes
RM = rm -rf 
SIMPLETHREAD = SimpleThread

.SUFFIXES: .java .class
.java.class:
	$(JC) $(JFLAGS) $*.java

CLASSES = \
	ChirpControllerDevice.java \
	SimpleThread.java \

default: dir classes

dir:
	mkdir -p $(CLASSESDIR)


classes: $(CLASSES:.java=.class)
	@- echo "Done Compiling!!"

run:
	$(JVM) -cp classes $(SIMPLETHREAD)


clean:
	$(RM) *.class classes
```

```sh
javac -d . ChirpControllerDevice.java SimpleThread.java
java SimpleThread
```