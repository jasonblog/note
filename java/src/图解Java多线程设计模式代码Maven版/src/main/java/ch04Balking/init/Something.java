package ch04Balking.init;

public class Something {
    private boolean initialized = false;

    public synchronized void init() {
        if (initialized) {
            return;
        }
        doInit();
        initialized = true;
    }

    private void doInit() {
        // 实际的初始化处理
    }
}