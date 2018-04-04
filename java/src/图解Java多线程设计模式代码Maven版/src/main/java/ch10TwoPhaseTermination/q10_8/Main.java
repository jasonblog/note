package ch10TwoPhaseTermination.q10_8;

public class Main {
    public static void main(String[] args) {
        // 创建线程
        Thread t = new Thread() {
            public void run() {
                while (true) {
                    try {
                        if (Thread.currentThread().isInterrupted()) {
                            throw new InterruptedException();
                        }
                        System.out.print(".");
                    } catch (InterruptedException e) {
                        System.out.print("*");
                    }
                }
            }
        };

        // 启动线程
        t.start();

        // 等待5秒
        try {
            Thread.sleep(5000);
        } catch (InterruptedException e) {
        }

        // 只interrupt线程一次
        t.interrupt();
    }
}
