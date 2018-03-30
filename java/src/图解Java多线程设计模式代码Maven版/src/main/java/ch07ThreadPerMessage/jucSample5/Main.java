package ch07ThreadPerMessage.jucSample5;

import java.util.concurrent.Executor;

public class Main {
    public static void main(String[] args) {
        System.out.println("main BEGIN");
        Host host = new Host(
                new Executor() {
                    public void execute(Runnable r) {
                        new Thread(r).start();
                    }
                }
        );
        host.request(10, 'A');
        host.request(20, 'B');
        host.request(30, 'C');
        System.out.println("main END");
    }
}
