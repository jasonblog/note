package ch10TwoPhaseTermination.a10_7;

public class HanoiThread extends Thread {
    // 发出终止请求后变为true
    private volatile boolean shutdownRequested = false;
    // 发出终止请求的时间
    private volatile long requestedTimeMillis = 0;

    // 终止请求
    public void shutdownRequest() {
        requestedTimeMillis = System.currentTimeMillis();
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
            for (int level = 0; !isShutdownRequested(); level++) {
                System.out.println("==== Level " + level + " ====");
                doWork(level, 'A', 'B', 'C');
                System.out.println("");
            }
        } catch (InterruptedException e) {
        } finally {
            doShutdown();
        }
    }

    // 操作
    private void doWork(int level, char posA, char posB, char posC) throws InterruptedException {
        if (level > 0) {
            if (isShutdownRequested()) {
                throw new InterruptedException();
            }
            doWork(level - 1, posA, posC, posB);
            System.out.print(posA + "->" + posB + " ");
            doWork(level - 1, posC, posB, posA);
        }
    }

    // 终止处理
    private void doShutdown() {
        long time = System.currentTimeMillis() - requestedTimeMillis;
        System.out.println("doShutdown: Latency = " + time + " msec.");
    }
}
