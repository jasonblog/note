package ch07ThreadPerMessage.a7_7b;

public class Blackhole {
    public static void enter(Object obj) {
        System.out.println("Step 1");
        magic(obj);
        System.out.println("Step 2");
        synchronized (obj) {
            System.out.println("Step 3 (never reached here)");  // 不会执行到这里
        }
    }

    public static void magic(final Object obj) {
        // 线程thread获取obj的锁之后，一直等待自身终止
        Thread thread = new Thread() {
            public void run() {
                synchronized (obj) {        // 在此处获取obj的锁
                    synchronized (this) {
                        this.notifyAll();   // 通知已经获取了obj的锁
                    }
                    try {
                        this.join(); // 一直等待
                    } catch (InterruptedException e) {
                    }
                }
            }
        };
        synchronized (thread) {
            thread.start();     // 启动线程
            try {
                thread.wait();  // 等待新的线程获取obj的锁
            } catch (InterruptedException e) {
            }
        }
    }
}
