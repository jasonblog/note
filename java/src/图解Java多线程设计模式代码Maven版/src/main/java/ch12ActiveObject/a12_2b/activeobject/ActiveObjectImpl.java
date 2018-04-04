package ch12ActiveObject.a12_2b.activeobject;

import java.math.BigInteger;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

// ActiveObject接口的实现类
class ActiveObjectImpl implements ActiveObject {
    private final ExecutorService service = Executors.newSingleThreadExecutor();

    // 终止服务
    public void shutdown() {
        service.shutdown();
    }

    // 有返回值的调用
    public Future<String> makeString(final int count, final char fillchar) {
        // 请求
        class MakeStringRequest implements Callable<String> {
            public String call() {
                char[] buffer = new char[count];
                for (int i = 0; i < count; i++) {
                    buffer[i] = fillchar;
                    try {
                        Thread.sleep(100);
                    } catch (InterruptedException e) {
                    }
                }
                return new String(buffer);
            }
        }
        // 发出请求
        return service.submit(new MakeStringRequest());
    }

    // 没有返回值的调用
    public void displayString(final String string) {
        // 请求
        class DisplayStringRequest implements Runnable {
            public void run() {
                try {
                    System.out.println("displayString: " + string);
                    Thread.sleep(10);
                } catch (InterruptedException e) {
                }
            }
        }
        // 发出请求
        service.execute(new DisplayStringRequest());
    }

    // 有返回值的调用
    public Future<String> add(final String x, final String y) {
        // 请求
        class AddRequest implements Callable<String> {
            public String call() throws NumberFormatException {
                BigInteger bigX = new BigInteger(x);
                BigInteger bigY = new BigInteger(y);
                BigInteger bigZ = bigX.add(bigY);
                return bigZ.toString();
            }
        }
        // 发出请求
        return service.submit(new AddRequest());
    }
}
