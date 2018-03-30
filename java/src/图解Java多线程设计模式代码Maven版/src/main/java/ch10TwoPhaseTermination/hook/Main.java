package ch10TwoPhaseTermination.hook;

public class Main {
    public static void main(String[] args) {
        System.out.println("main:BEGIN");

        // (1) 设置未捕获的异常的处理器
        Thread.setDefaultUncaughtExceptionHandler(
                new Thread.UncaughtExceptionHandler() {
                    public void uncaughtException(Thread thread, Throwable exception) {
                        System.out.println("****");
                        System.out.println("UncaughtExceptionHandler:BEGIN");
                        System.out.println("currentThread = " + Thread.currentThread());
                        System.out.println("thread = " + thread);
                        System.out.println("exception = " + exception);
                        System.out.println("UncaughtExceptionHandler:END");
                    }
                }
        );

        // (2) 设置退出钩子
        Runtime.getRuntime().addShutdownHook(
                new Thread() {
                    public void run() {
                        System.out.println("****");
                        System.out.println("shutdown hook:BEGIN");
                        System.out.println("currentThread = " + Thread.currentThread());
                        System.out.println("shutdown hook:END");
                    }
                }
        );

        // (3) 大约 3 秒后启动执行“整数除零计算”的线程
        new Thread("MyThread") {
            public void run() {
                System.out.println("MyThread:BEGIN");
                System.out.println("MyThread:SLEEP...");

                try {
                    Thread.sleep(3000);
                } catch (InterruptedException e) {
                }

                System.out.println("MyThread:DIVIDE");

                // 「整数除零计算」
                int x = 1 / 0;

                // 不会来到这里
                System.out.println("MyThread:END");
            }
        }.start();

        System.out.println("main:END");
    }
}
