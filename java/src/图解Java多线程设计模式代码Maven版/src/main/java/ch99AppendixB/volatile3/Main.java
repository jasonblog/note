package ch99AppendixB.volatile3;

class Something {
    private int x = 0;
    private volatile boolean valid = false;

    public void write() {
        x = 123;
        valid = true;
    }

    public void read() {
        if (valid) {
            System.out.println(x);
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
