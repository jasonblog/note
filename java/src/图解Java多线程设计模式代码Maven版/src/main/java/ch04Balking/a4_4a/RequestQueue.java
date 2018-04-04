package ch04Balking.a4_4a;

import java.util.LinkedList;
import java.util.Queue;

public class RequestQueue {
    private static final long TIMEOUT = 30000L;
    private final Queue<Request> queue = new LinkedList<Request>();

    public synchronized Request getRequest() {
        long start = System.currentTimeMillis(); // 开始时间
        while (queue.peek() == null) {
            long now = System.currentTimeMillis(); // 当前时间
            long rest = TIMEOUT - (now - start); // 剩余的等待时间
            if (rest <= 0) {
                throw new LivenessException("thrown by " + Thread.currentThread().getName());
            }
            try {
                wait(rest);
            } catch (InterruptedException e) {
            }
        }
        return queue.remove();
    }

    public synchronized void putRequest(Request request) {
        queue.offer(request);
        notifyAll();
    }
}
