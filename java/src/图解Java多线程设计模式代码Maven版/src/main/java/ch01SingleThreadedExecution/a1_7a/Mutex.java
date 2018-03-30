package ch01SingleThreadedExecution.a1_7a;

public final class Mutex {
    private boolean busy = false;

    public synchronized void lock() {
        while (busy) {
            try {
                wait();
            } catch (InterruptedException e) {
            }
        }
        busy = true;
    }

    public synchronized void unlock() {
        busy = false;
        notifyAll();
    }
}
