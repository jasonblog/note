package ch04Balking.a4_2a;

import java.io.IOException;
import java.util.Random;

public class ChangerThread extends Thread {
    private final Data data;
    private final Random random = new Random();

    public ChangerThread(String name, Data data) {
        super(name);
        this.data = data;
    }

    public void run() {
        try {
            for (int i = 0; true; i++) {
                data.change("No." + i);             // 修改数据
                Thread.sleep(random.nextInt(1000)); // 执行其他操作
                data.save();                        // 显式地保存
            }
        } catch (IOException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}