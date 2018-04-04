package ch04Balking.timeout;

import java.util.concurrent.TimeoutException;

public class Host {
    private final long timeout; // 超时时间
    private boolean ready = false; // 方法正常执行时值为true

    public Host(long timeout) {
        this.timeout = timeout;
    }

    // 修改状态
    public synchronized void setExecutable(boolean on) {
        ready = on;
        notifyAll();
    }

    // 检查状态之后再执行
    public synchronized void execute() throws InterruptedException, TimeoutException {
        long start = System.currentTimeMillis(); // 开始时间
        while (!ready) {
            long now = System.currentTimeMillis(); // 当前时间
            long rest = timeout - (now - start); // 剩余的等待时间
            if (rest <= 0) {
                throw new TimeoutException("now - start = " + (now - start) + ", timeout = " + timeout);
            }
            wait(rest);
        }
        doExecute();
    }

    // 实际的处理
    private void doExecute() {
        System.out.println(Thread.currentThread().getName() + " calls doExecute");
    }
}