package ch01SingleThreadedExecution.a1_5;

public class Main {
    public static void main(String[] args) {
        System.out.println("Testing SecurityGate...");
        for (int trial = 0; true; trial++) {
            SecurityGate gate = new SecurityGate();
            CrackerThread[] t = new CrackerThread[5];

            // 启动CrackerThread
            for (int i = 0; i < t.length; i++) {
                t[i] = new CrackerThread(gate);
                t[i].start();
            }

            // 等待CrackerThread终止
            for (int i = 0; i < t.length; i++) {
                try {
                    t[i].join();
                } catch (InterruptedException e) {
                }
            }

            // 确认
            if (gate.getCounter() == 0) {
                // 无矛盾
                System.out.print(".");
            } else {
                // 发现矛盾
                System.out.println("SecurityGate is NOT safe!");
                System.out.println("getCounter() == " + gate.getCounter());
                System.out.println("trial = " + trial);
                break;
            }
        }
    }
}
/*
Testing SecurityGate...
...........................................................................................................................................................................................................................................................SecurityGate is NOT safe!
getCounter() == 1
trial = 251
 */