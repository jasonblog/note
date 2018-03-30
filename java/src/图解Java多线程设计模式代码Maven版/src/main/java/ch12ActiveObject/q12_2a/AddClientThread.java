package ch12ActiveObject.q12_2a;

import ch12ActiveObject.a12_2a.activeobject.ActiveObject;
import ch12ActiveObject.a12_2a.activeobject.Result;

public class AddClientThread extends Thread {
    private final ActiveObject activeObject;
    private String x = "1";
    private String y = "1";

    public AddClientThread(String name, ActiveObject activeObject) {
        super(name);
        this.activeObject = activeObject;
    }

    public void run() {
        try {
            for (int i = 0; true; i++) {
                // 有返回值的调用
                Result<String> result = activeObject.add(x, y);
                Thread.sleep(100);
                String z = result.getResultValue();
                System.out.println(Thread.currentThread().getName() + ": " + x + " + " + y + " = " + z);
                x = y;
                y = z;
            }
        } catch (InterruptedException e) {
        }
    }
}
