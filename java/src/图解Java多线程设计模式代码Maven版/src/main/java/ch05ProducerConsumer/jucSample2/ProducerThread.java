package ch05ProducerConsumer.jucSample2;

import java.util.Random;
import java.util.concurrent.Exchanger;

public class ProducerThread extends Thread {
    private final Exchanger<char[]> exchanger;
    private final Random random;
    private char[] buffer = null;
    private char index = 0;

    public ProducerThread(Exchanger<char[]> exchanger, char[] buffer, long seed) {
        super("ProducerThread");
        this.exchanger = exchanger;
        this.buffer = buffer;
        this.random = new Random(seed);
    }

    public void run() {
        try {
            while (true) {
                // 向缓冲区填充字符
                for (int i = 0; i < buffer.length; i++) {
                    buffer[i] = nextChar();
                    System.out.println(Thread.currentThread().getName() + ": " + buffer[i] + " -> ");
                }

                // 交换缓冲区
                System.out.println(Thread.currentThread().getName() + ": BEFORE exchange");
                buffer = exchanger.exchange(buffer);
                System.out.println(Thread.currentThread().getName() + ": AFTER exchange");
            }
        } catch (InterruptedException e) {
        }
    }

    // 生成字符
    private char nextChar() throws InterruptedException {
        char c = (char) ('A' + index % 26);
        index++;
        Thread.sleep(random.nextInt(1000));
        return c;
    }
}
