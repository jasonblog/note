package ch99AppendixB.volatile2;

class Runner extends Thread {
    private volatile boolean quit = false;

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
        runner.start();
        runner.shutdown();
    }
}
