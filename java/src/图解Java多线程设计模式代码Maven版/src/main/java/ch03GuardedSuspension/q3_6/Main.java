package ch03GuardedSuspension.q3_6;

public class Main {
    public static void main(String[] args) {
        // 启动线程
        RequestQueue requestQueue = new RequestQueue();
        Thread alice = new ClientThread(requestQueue, "Alice", 314159L);
        Thread bobby = new ServerThread(requestQueue, "Bobby", 265358L);
        alice.start();
        bobby.start();

        // 等待约10秒
        try {
            Thread.sleep(10000);
        } catch (InterruptedException e) {
        }

        // 调用interrupt方法
        System.out.println("***** calling interrupt *****");
        alice.interrupt();
        bobby.interrupt();
    }
}
/*
Alice requests [ Request No.0 ]
Bobby handles  [ Request No.0 ]
Alice requests [ Request No.1 ]
Bobby handles  [ Request No.1 ]
Alice requests [ Request No.2 ]
Alice requests [ Request No.3 ]
Alice requests [ Request No.4 ]
Bobby handles  [ Request No.2 ]
Alice requests [ Request No.5 ]
Bobby handles  [ Request No.3 ]
Alice requests [ Request No.6 ]
...
 */