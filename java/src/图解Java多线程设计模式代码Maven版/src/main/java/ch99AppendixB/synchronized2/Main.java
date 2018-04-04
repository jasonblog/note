package ch99AppendixB.synchronized2;

class Something {
    private int x = 0;
    private int y = 0;

    public synchronized void write() {
        x = 100;
        y = 50;
    }

    public synchronized void read() {
        if (x < y) {
            System.out.println("x < y");
        }
    }
}

public class Main {
    public static void main(String[] args) {
        final Something obj = new Something();

        // 写数据的线程A
        new Thread() {
            public void run() {
                obj.write();
            }
        }.start();

        // 读数据的线程B
        new Thread() {
            public void run() {
                obj.read();
            }
        }.start();
    }
}
