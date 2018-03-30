package ch03GuardedSuspension.a3_6;

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
Bobby handles  [ Request No.4 ]
Alice requests [ Request No.7 ]
Bobby handles  [ Request No.5 ]
Bobby handles  [ Request No.6 ]
Alice requests [ Request No.8 ]
Alice requests [ Request No.9 ]
Alice requests [ Request No.10 ]
Alice requests [ Request No.11 ]
Bobby handles  [ Request No.7 ]
Bobby handles  [ Request No.8 ]
Alice requests [ Request No.12 ]
Bobby handles  [ Request No.9 ]
Alice requests [ Request No.13 ]
Bobby handles  [ Request No.10 ]
Bobby handles  [ Request No.11 ]
Bobby handles  [ Request No.12 ]
Alice requests [ Request No.14 ]
Alice requests [ Request No.15 ]
Bobby handles  [ Request No.13 ]
Alice requests [ Request No.16 ]
Bobby handles  [ Request No.14 ]
Bobby handles  [ Request No.15 ]
Alice requests [ Request No.17 ]
Bobby handles  [ Request No.16 ]
***** calling interrupt *****
 */