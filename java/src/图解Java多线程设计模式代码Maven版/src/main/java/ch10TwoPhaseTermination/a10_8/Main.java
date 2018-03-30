package ch10TwoPhaseTermination.a10_8;

public class Main {
    public static void main(String[] args) {
        // 创建线程
        Thread thread = new Thread() {
            public void run() {
                while (true) {
                    try {
                        if (Thread.interrupted()) {
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
        thread.start();

        // 等待5秒
        try {
            Thread.sleep(5000);
        } catch (InterruptedException e) {
        }

        // 只interrupt线程一次
        thread.interrupt();
    }
}
