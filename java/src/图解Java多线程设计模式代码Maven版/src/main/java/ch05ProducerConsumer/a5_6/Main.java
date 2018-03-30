package ch05ProducerConsumer.a5_6;

public class Main {
    public static void main(String[] args) {
        Table table = new Table(3);     // 创建一个能放置3个蛋糕的桌子
        Thread[] threads = {
                new MakerThread("MakerThread-1", table, 31415),
                new MakerThread("MakerThread-2", table, 92653),
                new MakerThread("MakerThread-3", table, 58979),
                new EaterThread("EaterThread-1", table, 32384),
                new EaterThread("EaterThread-2", table, 62643),
                new EaterThread("EaterThread-3", table, 38327),
        };

        // 启动线程
        for (int i = 0; i < threads.length; i++) {
            threads[i].start();
        }

        // 休眠约10秒
        try {
            Thread.sleep(10000);
        } catch (InterruptedException e) {
        }

        System.out.println("***** interrupt *****");

        // 中断
        for (int i = 0; i < threads.length; i++) {
            threads[i].interrupt();
        }
    }
}
