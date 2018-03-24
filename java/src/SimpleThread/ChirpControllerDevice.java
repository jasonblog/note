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
