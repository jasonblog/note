package ch00Introduction1.printThread;

public class PrintThread extends Thread {
    private String message;

    public PrintThread(String message) {
        this.message = message;
    }

    public void run() {
        for (int i = 0; i < 10000; i++) {
            System.out.print(message);
        }
    }
}