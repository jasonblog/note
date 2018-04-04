package ch05ProducerConsumer.a5_9;

public class Main {
    public static void main(String[] args) {
        System.out.println("BEGIN");
        try {
            Something.method(3000);
        } catch (InterruptedException e) {
        }
        System.out.println("END");
    }
}
