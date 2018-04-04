package ch00Introduction1.twoThreads;

public class Main {
    public static void main(String[] args) {
        MyThread t = new MyThread();
        t.start();
        for (int i = 0; i < 10000; i++) {
            System.out.print("Good!");
        }
    }
}
