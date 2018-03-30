package ch01SingleThreadedExecution.q1_7a;

import ch01SingleThreadedExecution.a1_7a.Mutex;

public class Gate {
    private final Mutex mutex = new Mutex();
    private int counter = 0;
    private String name = "Nobody";
    private String address = "Nowhere";

    public void pass(String name, String address) { // 非synchronized方法
        mutex.lock();
        try {
            this.counter++;
            this.name = name;
            this.address = address;
            check();
        } finally {
            mutex.unlock();
        }
    }

    public String toString() { // 非synchronized方法
        String s = null;
        mutex.lock();
        try {
            s = "No." + counter + ": " + name + ", " + address;
        } finally {
            mutex.unlock();
        }
        return s;
    }

    private void check() {
        if (name.charAt(0) != address.charAt(0)) {
            System.out.println("***** BROKEN ***** " + toString());
        }
    }
}
/*

 */