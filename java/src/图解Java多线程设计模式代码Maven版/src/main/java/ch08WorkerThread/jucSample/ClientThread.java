package ch08WorkerThread.jucSample;

import java.util.Random;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.RejectedExecutionException;

public class ClientThread extends Thread {
    private static final Random random = new Random();
    private final ExecutorService executorService;

    public ClientThread(String name, ExecutorService executorService) {
        super(name);
        this.executorService = executorService;
    }

    public void run() {
        try {
            for (int i = 0; true; i++) {
                Request request = new Request(getName(), i);
                executorService.execute(request);
                Thread.sleep(random.nextInt(1000));
            }
        } catch (InterruptedException e) {
        } catch (RejectedExecutionException e) {
            System.out.println(getName() + " : " + e);
        }
    }
}
