package ch10TwoPhaseTermination.q10_4;

public class GracefulThread extends Thread {
    // 发出终止请求后变为true
    private volatile boolean shutdownRequested = false;

    // 终止请求
    public final void shutdownRequest() {
        shutdownRequested = true;
        interrupt();
    }

    // 检查是否已经发出了终止请求
    public final boolean isShutdownRequested() {
        return shutdownRequested;
    }

    // 线程体
    public final void run() {
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
    protected void doWork() throws InterruptedException {
    }

    // 终止处理
    protected void doShutdown() {
    }
}
