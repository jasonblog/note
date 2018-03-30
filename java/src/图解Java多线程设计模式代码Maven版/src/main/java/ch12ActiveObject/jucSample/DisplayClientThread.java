package ch12ActiveObject.jucSample;

import ch12ActiveObject.jucSample.activeobject.ActiveObject;

import java.util.concurrent.CancellationException;
import java.util.concurrent.RejectedExecutionException;

public class DisplayClientThread extends Thread {
    private final ActiveObject activeObject;

    public DisplayClientThread(String name, ActiveObject activeObject) {
        super(name);
        this.activeObject = activeObject;
    }

    public void run() {
        try {
            for (int i = 0; true; i++) {
                // 有返回值的调用
                String string = Thread.currentThread().getName() + " " + i;
                activeObject.displayString(string);
                Thread.sleep(200);
            }
        } catch (RejectedExecutionException e) {
            System.out.println(Thread.currentThread().getName() + ":" + e);
        } catch (CancellationException e) {
            System.out.println(Thread.currentThread().getName() + ":" + e);
        } catch (InterruptedException e) {
            System.out.println(Thread.currentThread().getName() + ":" + e);
        }
    }
}
