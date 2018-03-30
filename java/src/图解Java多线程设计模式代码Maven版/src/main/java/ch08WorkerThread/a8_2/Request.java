package ch08WorkerThread.a8_2;

import java.util.Random;

public class Request {
    private static final Random random = new Random();
    private final String name; // 委托者
    private final int number;  // 请求的编号

    public Request(String name, int number) {
        this.name = name;
        this.number = number;
    }

    public void execute() {
        System.out.println(Thread.currentThread().getName() + " executes " + this);
        try {
            Thread.sleep(random.nextInt(1000));
        } catch (InterruptedException e) {
        }
    }

    public String toString() {
        return "[ Request from " + name + " No." + number + " ]";
    }
}
