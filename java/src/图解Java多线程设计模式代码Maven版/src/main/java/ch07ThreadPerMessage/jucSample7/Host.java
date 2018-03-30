package ch07ThreadPerMessage.jucSample7;

import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class Host {
    private final Helper helper = new Helper();
    private final ScheduledExecutorService scheduledExecutorService;

    public Host(ScheduledExecutorService scheduledExecutorService) {
        this.scheduledExecutorService = scheduledExecutorService;
    }

    public void request(final int count, final char c) {
        System.out.println("    request(" + count + ", " + c + ") BEGIN");
        scheduledExecutorService.schedule(
                new Runnable() {
                    public void run() {
                        helper.handle(count, c);
                    }
                },
                3L,
                TimeUnit.SECONDS
        );
        System.out.println("    request(" + count + ", " + c + ") END");
    }
}
