package ch06ReadWriteLock.q6_6;

public final class ReadWriteLock {
    private int readingReaders = 0; // (a)…实际正在读取的线程个数
    private int writingWriters = 0; // (b)…实际正在写入的线程个数

    public synchronized void readLock() throws InterruptedException {
        while (writingWriters > 0) {
            wait();
        }
        readingReaders++;                       // (a) 实际正在读取的线程个数加1
    }

    public synchronized void readUnlock() {
        readingReaders--;                       // (a) 实际正在读取的线程个数减1
        notifyAll();
    }

    public synchronized void writeLock() throws InterruptedException {
        while (readingReaders > 0 || writingWriters > 0) {
            wait();
        }
        writingWriters++;                       // (b) 实际正在写入的线程个数加1
    }

    public synchronized void writeUnlock() {
        writingWriters--;                       // (b) 实际正在写入的线程个数减1
        notifyAll();
    }
}
