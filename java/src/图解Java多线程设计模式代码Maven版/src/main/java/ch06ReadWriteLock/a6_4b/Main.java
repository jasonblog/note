package ch06ReadWriteLock.a6_4b;

public class Main {
    public static void main(String[] args) {
        Database<String, String> database = new Database<String, String>();

        // 启动AssignThread线程
        new AssignThread(database, "Alice", "Alaska").start();
        new AssignThread(database, "Alice", "Australia").start();
        new AssignThread(database, "Bobby", "Brazil").start();
        new AssignThread(database, "Bobby", "Bulgaria").start();

        // 启动RetrieveThread线程
        for (int i = 0; i < 100; i++) {
            new RetrieveThread(database, "Alice").start();
            new RetrieveThread(database, "Bobby").start();
        }

        // 停止约10秒
        try {
            Thread.sleep(10000);
        } catch (InterruptedException e) {
        }

        // 强制终止
        System.exit(0);
    }
}
