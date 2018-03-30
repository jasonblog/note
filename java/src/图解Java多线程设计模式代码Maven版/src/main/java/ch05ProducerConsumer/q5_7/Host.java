package ch05ProducerConsumer.q5_7;

public class Host {
    public static void execute(int count) {
        for (int i = 0; i < count; i++) {
            doHeavyJob();
        }
    }

    private static void doHeavyJob() {
        // 下面代码用于表示"无法取消的繁重处理"（循环处理约10秒）
        System.out.println("doHeavyJob BEGIN");
        long start = System.currentTimeMillis();
        while (start + 10000 > System.currentTimeMillis()) {
            // busy loop
        }
        System.out.println("doHeavyJob END");
    }
}
