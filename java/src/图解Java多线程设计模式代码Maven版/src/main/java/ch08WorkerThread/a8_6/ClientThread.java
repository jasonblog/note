package ch08WorkerThread.a8_6;

import java.util.Random;

public class ClientThread extends Thread {
    private static final Random random = new Random();
    private final Channel channel;
    private volatile boolean terminated = false;

    public ClientThread(String name, Channel channel) {
        super(name);
        this.channel = channel;
    }

    public void run() {
        try {
            for (int i = 0; !terminated; i++) {
                try {
                    Request request = new Request(getName(), i);
                    channel.putRequest(request);
                    Thread.sleep(random.nextInt(1000));
                } catch (InterruptedException e) {
                    terminated = true;
                }
            }
        } finally {
            System.out.println(Thread.currentThread().getName() + " is terminated.");
        }
    }

    public void stopThread() {
        terminated = true;
        interrupt();
    }
}
