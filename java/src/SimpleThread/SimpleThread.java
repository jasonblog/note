import com.abc.ChirpControllerDevice;

public class SimpleThread
{
    public static void main(String[] args)
    {
        ChirpControllerDevice mC0 = null;

        Thread thread = new Thread(new Runnable() {
            public void run() {
                while (true) {
                    try {
                        Thread.sleep(5000);
                    } catch (InterruptedException e) {
                        System.out.println(e);
                    }

                    System.out.println("T");
                }

            }
        });
        thread.start();

        mC0 = new ChirpControllerDevice();
        mC0.init();

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
