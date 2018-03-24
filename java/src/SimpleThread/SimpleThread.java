import com.abc.ChirpControllerDevice;

public class SimpleThread
{
    public static void main(String[] args)
    {
        ChirpControllerDevice mC0 = null;
        ChirpControllerDevice mC1 = null;

        Thread thread = new Thread(new Runnable() {
            public long getPID() {
                String processName =
                    java.lang.management.ManagementFactory.getRuntimeMXBean().getName();
                return Long.parseLong(processName.split("@")[0]);
            }

            public void run() {
                while (true) {
                    try {
                        Thread.sleep(5000);
                    } catch (InterruptedException e) {
                        System.out.println(e);
                    }

                    System.out.println("T" + ", PID:" + getPID() + ", TID:"+ Thread.currentThread().getId());
                }

            }
        });
        thread.start();

        mC0 = new ChirpControllerDevice(88);
        mC0.init();
        mC1 = new ChirpControllerDevice(66);
        mC1.init();

        /*
        while (true) {
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                System.out.println(e);
            }
        }
        */

        // 主執行緒繼續進行其它工作........
        // such...such....
        // 現在主執行緒執行到這邊了，工作應該結束了
    }
}
