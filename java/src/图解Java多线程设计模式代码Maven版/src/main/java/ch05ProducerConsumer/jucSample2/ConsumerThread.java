package ch05ProducerConsumer.jucSample2;

import java.util.Random;
import java.util.concurrent.Exchanger;

public class ConsumerThread extends Thread {
    private final Exchanger<char[]> exchanger;
    private final Random random;
    private char[] buffer = null;

    public ConsumerThread(Exchanger<char[]> exchanger, char[] buffer, long seed) {
        super("ConsumerThread");
        this.exchanger = exchanger;
        this.buffer = buffer;
        this.random = new Random(seed);
    }

    public void run() {
        try {
            while (true) {
                // 交换缓冲区
                System.out.println(Thread.currentThread().getName() + ": BEFORE exchange");
                buffer = exchanger.exchange(buffer);
                System.out.println(Thread.currentThread().getName() + ": AFTER exchange");

                // 从缓冲区中取出字符
                for (int i = 0; i < buffer.length; i++) {
                    System.out.println(Thread.currentThread().getName() + ":  -> " + buffer[i]);
                    Thread.sleep(random.nextInt(1000));
                }
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
