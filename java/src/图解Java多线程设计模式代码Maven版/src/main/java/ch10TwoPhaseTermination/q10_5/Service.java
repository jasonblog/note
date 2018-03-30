package ch10TwoPhaseTermination.q10_5;

public class Service {
    // 开始运行服务
    public static void service() {
        System.out.print("service");
        for (int i = 0; i < 50; i++) {
            System.out.print(".");
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
            }
        }
        System.out.println("done.");
    }

    // 停止服务
    public static void cancel() {
        // 未实现
    }
}
