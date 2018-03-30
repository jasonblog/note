package ch08WorkerThread.a8_3a;

public class Request {
    private final String name;
    private final int number;

    public Request(String name, int number) {
        this.name = name;
        this.number = number;
    }

    public void execute() {
        System.out.println(Thread.currentThread().getName() + " executes " + this);
    }

    public String toString() {
        return "[ Request from " + name + " No." + number + " ]";
    }
}
