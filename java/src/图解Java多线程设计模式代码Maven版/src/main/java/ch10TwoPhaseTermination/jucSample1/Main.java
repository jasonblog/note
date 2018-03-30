package ch10TwoPhaseTermination.jucSample1;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Main {
    private static final int TASKS = 10; // 工作的个数

    public static void main(String[] args) {
        System.out.println("BEGIN");
        ExecutorService service = Executors.newFixedThreadPool(5);
        CountDownLatch doneLatch = new CountDownLatch(TASKS);
        try {
            // 开始工作
            for (int t = 0; t < TASKS; t++) {
                service.execute(new MyTask(doneLatch, t));
            }
            System.out.println("AWAIT");
            // 等待工作结束
            doneLatch.await();
        } catch (InterruptedException e) {
        } finally {
            service.shutdown();
            System.out.println("END");
        }
    }
}
