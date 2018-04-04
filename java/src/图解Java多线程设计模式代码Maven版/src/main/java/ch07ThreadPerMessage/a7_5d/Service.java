package ch07ThreadPerMessage.a7_5d;

public class Service {
    private static Thread worker = null;

    public static synchronized void service() {
        // 当存在执行中的处理时，使用interrupt将其取消
        if (worker != null && worker.isAlive()) {
            worker.interrupt();
            try {
                worker.join();
            } catch (InterruptedException e) {
            }
            worker = null;
        }
        System.out.print("service");
        worker = new Thread() {
            public void run() {
                doService();
            }
        };
        worker.start();
    }

    private static void doService() {
        try {
            for (int i = 0; i < 50; i++) {
                System.out.print(".");
                Thread.sleep(100);
            }
            System.out.println("done.");
        } catch (InterruptedException e) {
            System.out.println("cancelled.");
        }
    }
}
