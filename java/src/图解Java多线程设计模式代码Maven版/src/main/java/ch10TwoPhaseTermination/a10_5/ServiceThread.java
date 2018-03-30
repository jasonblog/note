package ch10TwoPhaseTermination.a10_5;

public class ServiceThread extends GracefulThread {
    private int count = 0;

    // 操作中
    @Override
    protected void doWork() throws InterruptedException {
        System.out.print(".");
        Thread.sleep(100);
        count++;
        if (count >= 50) {
            shutdownRequest();  // 自己终止
        }
    }

    // 终止处理
    @Override
    protected void doShutdown() {
        System.out.println("done.");
    }
}
