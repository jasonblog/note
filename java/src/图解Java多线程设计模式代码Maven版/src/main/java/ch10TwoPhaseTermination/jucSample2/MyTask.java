package ch10TwoPhaseTermination.jucSample2;

import java.util.Random;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.CyclicBarrier;

public class MyTask implements Runnable {
    private static final int PHASE = 5;
    private static final Random random = new Random(314159);
    private final CyclicBarrier phaseBarrier;
    private final CountDownLatch doneLatch;
    private final int context;

    public MyTask(CyclicBarrier phaseBarrier, CountDownLatch doneLatch, int context) {
        this.phaseBarrier = phaseBarrier;
        this.doneLatch = doneLatch;
        this.context = context;
    }

    public void run() {
        try {
            for (int phase = 0; phase < PHASE; phase++) {
                doPhase(phase);
                phaseBarrier.await();
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        } catch (BrokenBarrierException e) {
            e.printStackTrace();
        } finally {
            doneLatch.countDown();
        }
    }

    protected void doPhase(int phase) {
        String name = Thread.currentThread().getName();
        System.out.println(name + ":MyTask:BEGIN:context = " + context + ", phase = " + phase);
        try {
            Thread.sleep(random.nextInt(3000));
        } catch (InterruptedException e) {
        } finally {
            System.out.println(name + ":MyTask:END:context = " + context + ", phase = " + phase);
        }
    }
}
