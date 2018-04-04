package ch02Immutable.jucSample1;

import java.util.ArrayList;
import java.util.List;

public class Main {
    public static void main(String[] args) {
        List<Integer> list = new ArrayList<Integer>();
        new WriterThread(list).start();
        new ReaderThread(list).start();
    }
}
/*
Exception in thread "ReaderThread" java.util.ConcurrentModificationException
	at java.util.ArrayList$Itr.checkForComodification(ArrayList.java:901)
	at java.util.ArrayList$Itr.next(ArrayList.java:851)
	at ch02Immutable.jucSample1.ReaderThread.run(ReaderThread.java:15)
5175
 */