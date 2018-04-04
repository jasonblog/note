package ch99AppendixB.volatile1;

class Runner extends Thread {
    private boolean quit = false;

    public void run() {
        while (!quit) {
            // ...
        }
        System.out.println("Done");
    }

    public void shutdown() {
        quit = true;
    }
}

public class Main {
    public static void main(String[] args) {
        Runner runner = new Runner();

        // 启动线程
        runner.start();

        // 终止线程
        runner.shutdown();
    }
}
