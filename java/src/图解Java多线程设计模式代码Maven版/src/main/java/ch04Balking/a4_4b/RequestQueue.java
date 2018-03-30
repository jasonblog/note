package ch04Balking.a4_4b;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

public class RequestQueue {
    private final BlockingQueue<Request> queue = new LinkedBlockingQueue<Request>();

    public Request getRequest() {
        Request req = null;
        try {
            req = queue.poll(30L, TimeUnit.SECONDS);
            if (req == null) {
                throw new LivenessException("thrown by " + Thread.currentThread().getName());
            }
        } catch (InterruptedException e) {
        }
        return req;
    }

    public void putRequest(Request request) {
        try {
            boolean offered = queue.offer(request, 30L, TimeUnit.SECONDS);
            if (!offered) {
                throw new LivenessException("thrown by " + Thread.currentThread().getName());
            }
        } catch (InterruptedException e) {
        }
    }
}
