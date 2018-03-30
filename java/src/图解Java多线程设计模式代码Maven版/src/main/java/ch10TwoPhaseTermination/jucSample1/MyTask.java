package ch10TwoPhaseTermination.jucSample1;

import java.util.Random;
import java.util.concurrent.CountDownLatch;

public class MyTask implements Runnable {
    private static final Random random = new Random(314159);
    private final CountDownLatch doneLatch;
    private final int context;

    public MyTask(CountDownLatch doneLatch, int context) {
        this.doneLatch = doneLatch;
        this.context = context;
    }

    public void run() {
        doTask();
        doneLatch.countDown();
    }

    protected void doTask() {
        String name = Thread.currentThread().getName();
        System.out.println(name + ":MyTask:BEGIN:context = " + context);
        try {
            Thread.sleep(random.nextInt(3000));
        } catch (InterruptedException e) {
        } finally {
            System.out.println(name + ":MyTask:END:context = " + context);
        }
    }
}
