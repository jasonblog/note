package ch10TwoPhaseTermination.q10_7;

public class Main {
    public static void main(String[] args) {
        System.out.println("main: BEGIN");
        try {
            // 启动线程
            HanoiThread t = new HanoiThread();
            t.start();

            // 稍微间隔一段时间
            Thread.sleep(10000);

            // 线程的终止请求
            System.out.println("main: shutdownRequest");
            t.shutdownRequest();

            // 等待线程终止
            System.out.println("main: join");
            t.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        System.out.println("main: END");
    }
}
