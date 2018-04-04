package ch10TwoPhaseTermination.a10_3;

import java.io.FileWriter;
import java.io.IOException;

public class CountupThread extends Thread {
    // 计数值
    private long counter = 0;

    // 发出终止请求后变为true
    private volatile boolean shutdownRequested = false;

    // 终止请求
    public void shutdownRequest() {
        shutdownRequested = true;
        interrupt();
    }

    // 检查是否发出了终止请求
    public boolean isShutdownRequested() {
        return shutdownRequested;
    }

    // 线程体
    public void run() {
        try {
            while (!isShutdownRequested()) {
                doWork();
            }
        } catch (InterruptedException e) {
        } finally {
            doShutdown();
        }
    }

    // 操作
    private void doWork() throws InterruptedException {
        counter++;
        System.out.println("doWork: counter = " + counter);
        Thread.sleep(500);
    }

    // 终止处理
    private void doShutdown() {
        System.out.println("doShutdown: counter = " + counter);
        System.out.println("doShutdown: Save BEGIN");
        try {
            FileWriter writer = new FileWriter("counter.txt");
            writer.write("counter = " + counter);
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        System.out.println("doShutdown: Save END");
    }
}
