package ch11ThreadSpecificStorage.a11_3;

public class Log {
    private static final ThreadLocal<TSLog> tsLogCollection = new ThreadLocal<TSLog>();

    // 写日志
    public static void println(String s) {
        getTSLog().println(s);
    }

    // 关闭日志
    public static void close() {
        getTSLog().close();
    }

    // 获得线程特有的日志
    private static TSLog getTSLog() {
        TSLog tsLog = tsLogCollection.get();

        // 如果该线程是第一次调用本方法，就新生成并注册一个日志
        if (tsLog == null) {
            tsLog = new TSLog(Thread.currentThread().getName() + "-log.txt");
            tsLogCollection.set(tsLog);
            startWatcher(tsLog);
        }

        return tsLog;
    }

    // 启动一个等待该线程终止的线程
    private static void startWatcher(final TSLog tsLog) {
        // 被监视的线程
        final Thread target = Thread.currentThread();
        // 监视 target 的线程
        final Thread watcher = new Thread() {
            public void run() {
                System.out.println("startWatcher for " + target.getName() + " BEGIN");
                try {
                    target.join();
                } catch (InterruptedException e) {
                }
                tsLog.close();
                System.out.println("startWatcher for " + target.getName() + " END");
            }
        };
        // 开始监视
        watcher.start();
    }
}
