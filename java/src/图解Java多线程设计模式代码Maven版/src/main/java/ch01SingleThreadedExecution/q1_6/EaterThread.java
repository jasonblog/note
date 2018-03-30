package ch01SingleThreadedExecution.q1_6;

public class EaterThread extends Thread {
    private final Tool lefthand;
    private final Tool righthand;
    private String name;

    public EaterThread(String name, Tool lefthand, Tool righthand) {
        this.name = name;
        this.lefthand = lefthand;
        this.righthand = righthand;
    }

    public void run() {
        while (true) {
            eat();
        }
    }

    public void eat() {
        synchronized (lefthand) {
            System.out.println(name + " takes up " + lefthand + " (left).");
            synchronized (righthand) {
                System.out.println(name + " takes up " + righthand + " (right).");
                System.out.println(name + " is eating now, yum yum!");
                System.out.println(name + " puts down " + righthand + " (right).");
            }
            System.out.println(name + " puts down " + lefthand + " (left).");
        }
    }
}
