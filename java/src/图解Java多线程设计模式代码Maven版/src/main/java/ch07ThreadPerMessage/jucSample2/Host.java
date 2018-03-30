package ch07ThreadPerMessage.jucSample2;

public class Host {
    private final Helper helper = new Helper();

    public void request(final int count, final char c) {
        System.out.println("    request(" + count + ", " + c + ") BEGIN");
        new Thread(
                new Runnable() {
                    public void run() {
                        helper.handle(count, c);
                    }
                }
        ).start();
        System.out.println("    request(" + count + ", " + c + ") END");
    }
}
