package ch04Balking.timeout;

import java.util.concurrent.TimeoutException;

public class Main {
    public static void main(String[] args) {
        Host host = new Host(10000);
        try {
            System.out.println("execute BEGIN");
            host.execute();
        } catch (TimeoutException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
/*
execute BEGIN
java.util.concurrent.TimeoutException: now - start = 10000, timeout = 10000
	at ch04Balking.timeout.Host.execute(Host.java:26)
	at ch04Balking.timeout.Main.main(Main.java:10)

 */