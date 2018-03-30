package ch05ProducerConsumer.sample;

import java.util.Random;

public class MakerThread extends Thread {
    private static int id = 0; // 蛋糕的流水号(所有糕点师共用)
    private final Random random;
    private final Table table;

    public MakerThread(String name, Table table, long seed) {
        super(name);
        this.table = table;
        this.random = new Random(seed);
    }

    private static synchronized int nextId() {
        return id++;
    }

    public void run() {
        try {
            while (true) {
                Thread.sleep(random.nextInt(1000));
                String cake = "[ Cake No." + nextId() + " by " + getName() + " ]";
                table.put(cake);
            }
        } catch (InterruptedException e) {
        }
    }
}