package ch03GuardedSuspension.a3_3;

import java.util.LinkedList;
import java.util.Queue;

public class RequestQueue {
    private final Queue<Request> queue = new LinkedList<Request>();

    public synchronized Request getRequest() {
        while (queue.peek() == null) {
            try {
                System.out.println(Thread.currentThread().getName() + ": wait() begins, queue = " + queue);
                wait();
                System.out.println(Thread.currentThread().getName() + ": wait() ends,   queue = " + queue);
            } catch (InterruptedException e) {
            }
        }
        return queue.remove();
    }

    public synchronized void putRequest(Request request) {
        queue.offer(request);
        System.out.println(Thread.currentThread().getName() + ": notifyAll() begins, queue = " + queue);
        notifyAll();
        System.out.println(Thread.currentThread().getName() + ": notifyAll() ends, queue = " + queue);
    }
}
