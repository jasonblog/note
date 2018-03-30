package ch05ProducerConsumer.sample;

public class Main {
    public static void main(String[] args) {
        Table table = new Table(3);     // 创建一个能放置3个蛋糕的桌子
        new MakerThread("MakerThread-1", table, 31415).start();
        new MakerThread("MakerThread-2", table, 92653).start();
        new MakerThread("MakerThread-3", table, 58979).start();
        new EaterThread("EaterThread-1", table, 32384).start();
        new EaterThread("EaterThread-2", table, 62643).start();
        new EaterThread("EaterThread-3", table, 38327).start();
    }
}
/*
MakerThread-2 puts [ Cake No.0 by MakerThread-2 ]
EaterThread-1 takes [ Cake No.0 by MakerThread-2 ]
MakerThread-1 puts [ Cake No.1 by MakerThread-1 ]
EaterThread-2 takes [ Cake No.1 by MakerThread-1 ]
MakerThread-3 puts [ Cake No.2 by MakerThread-3 ]
EaterThread-3 takes [ Cake No.2 by MakerThread-3 ]
MakerThread-2 puts [ Cake No.3 by MakerThread-2 ]
EaterThread-1 takes [ Cake No.3 by MakerThread-2 ]
...
 */