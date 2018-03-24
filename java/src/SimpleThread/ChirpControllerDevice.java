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
