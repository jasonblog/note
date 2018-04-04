package ch01SingleThreadedExecution.jucSemaphore;

import java.util.Random;
import java.util.concurrent.Semaphore;

class Log {
    public static void println(String s) {
        System.out.println(Thread.currentThread().getName() + ": " + s);
    }
}

// 资源个数有限
class BoundedResource {
    private final static Random random = new Random(314159);
    private final Semaphore semaphore;
    private final int permits;

    // 构造函数(permits为资源个数)
    public BoundedResource(int permits) {
        this.semaphore = new Semaphore(permits);
        this.permits = permits;
    }

    // 使用资源
    public void use() throws InterruptedException {
        semaphore.acquire();
        try {
            doUse();
        } finally {
            semaphore.release();
        }
    }

    // 实际使用资源(此处仅使用Thread.sleep)
    protected void doUse() throws InterruptedException {
        Log.println("BEGIN: used = " + (permits - semaphore.availablePermits()));
        Thread.sleep(random.nextInt(500));
        Log.println("END:   used = " + (permits - semaphore.availablePermits()));
    }
}

// 使用资源的线程
class UserThread extends Thread {
    private final static Random random = new Random(26535);
    private final BoundedResource resource;

    public UserThread(BoundedResource resource) {
        this.resource = resource;
    }

    public void run() {
        try {
            while (true) {
                resource.use();
                Thread.sleep(random.nextInt(3000));
            }
        } catch (InterruptedException e) {
        }
    }
}

public class Main {
    public static void main(String[] args) {
        // 设置3个资源
        BoundedResource resource = new BoundedResource(3);

        // 10个线程使用资源
        for (int i = 0; i < 10; i++) {
            new UserThread(resource).start();
        }
    }
}
/*
Thread-0: BEGIN: used = 1
Thread-1: BEGIN: used = 2
Thread-2: BEGIN: used = 3
Thread-1: END:   used = 3
Thread-5: BEGIN: used = 3
Thread-5: END:   used = 3
Thread-4: BEGIN: used = 3
Thread-2: END:   used = 3
Thread-8: BEGIN: used = 3
Thread-4: END:   used = 3
Thread-6: BEGIN: used = 3
Thread-6: END:   used = 3
Thread-9: BEGIN: used = 3
Thread-0: END:   used = 3
Thread-3: BEGIN: used = 3
Thread-8: END:   used = 3
Thread-7: BEGIN: used = 3
Thread-3: END:   used = 3
Thread-7: END:   used = 2
Thread-9: END:   used = 2
Thread-1: BEGIN: used = 2
Thread-3: BEGIN: used = 2
Thread-5: BEGIN: used = 3
Thread-1: END:   used = 3
Thread-4: BEGIN: used = 3
Thread-3: END:   used = 3
Thread-5: END:   used = 2
Thread-4: END:   used = 1
Thread-9: BEGIN: used = 1
Thread-9: END:   used = 1
Thread-4: BEGIN: used = 1
Thread-4: END:   used = 1
Thread-8: BEGIN: used = 1
Thread-1: BEGIN: used = 2
Thread-1: END:   used = 2
Thread-4: BEGIN: used = 2
Thread-8: END:   used = 2
Thread-2: BEGIN: used = 2
Thread-7: BEGIN: used = 3
Thread-4: END:   used = 3
Thread-0: BEGIN: used = 3
Thread-2: END:   used = 3
Thread-0: END:   used = 2
Thread-5: BEGIN: used = 2
Thread-3: BEGIN: used = 3
Thread-7: END:   used = 3
Thread-6: BEGIN: used = 3
Thread-6: END:   used = 3
Thread-1: BEGIN: used = 3
Thread-3: END:   used = 3
Thread-7: BEGIN: used = 3
Thread-5: END:   used = 3
Thread-4: BEGIN: used = 3
Thread-1: END:   used = 3
Thread-4: END:   used = 2
Thread-3: BEGIN: used = 2
Thread-0: BEGIN: used = 3
Thread-7: END:   used = 3
Thread-7: BEGIN: used = 3
 */