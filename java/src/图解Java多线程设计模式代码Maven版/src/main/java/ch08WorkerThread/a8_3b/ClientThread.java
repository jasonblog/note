package ch08WorkerThread.a8_3b;

public class ClientThread extends Thread {
    private final Channel channel;

    public ClientThread(String name, Channel channel) {
        super(name);
        this.channel = channel;
    }

    public void run() {
        for (int i = 0; true; i++) {
            Request request = new Request(getName(), i);
            channel.putRequest(request);
        }
    }
}
