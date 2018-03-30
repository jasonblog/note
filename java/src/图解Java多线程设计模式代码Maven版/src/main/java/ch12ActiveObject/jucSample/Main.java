package ch12ActiveObject.jucSample;

import ch12ActiveObject.jucSample.activeobject.ActiveObject;
import ch12ActiveObject.jucSample.activeobject.ActiveObjectFactory;

public class Main {
    public static void main(String[] args) {
        ActiveObject activeObject = ActiveObjectFactory.createActiveObject();
        try {
            new MakerClientThread("Alice", activeObject).start();
            new MakerClientThread("Bobby", activeObject).start();
            new DisplayClientThread("Chris", activeObject).start();
            Thread.sleep(5000);
        } catch (InterruptedException e) {
        } finally {
            System.out.println("*** shutdown ***");
            activeObject.shutdown();
        }
    }
}
/*
displayString: Chris 0
Bobby: value =
Alice: value =
Bobby: value = B
Alice: value = A
displayString: Chris 1
Bobby: value = BB
Alice: value = AA
Bobby: value = BBB
displayString: Chris 2
Alice: value = AAA
displayString: Chris 3
displayString: Chris 4
displayString: Chris 5
Bobby: value = BBBB
Alice: value = AAAA
displayString: Chris 6
displayString: Chris 7
Bobby: value = BBBBB
displayString: Chris 8
displayString: Chris 9
displayString: Chris 10
Alice: value = AAAAA
displayString: Chris 11
displayString: Chris 12
displayString: Chris 13
Bobby: value = BBBBBB
displayString: Chris 14
displayString: Chris 15
Alice: value = AAAAAA
displayString: Chris 16
displayString: Chris 17
displayString: Chris 18
*** shutdown ***
Chris:java.util.concurrent.RejectedExecutionException: Task ch12ActiveObject.jucSample.activeobject.ActiveObjectImpl$1DisplayStringRequest@61173874 rejected from java.util.concurrent.ThreadPoolExecutor@6fd0da26[Shutting down, pool size = 1, active threads = 1, queued tasks = 7, completed tasks = 33]
displayString: Chris 19
Bobby: value = BBBBBBB
Bobby:java.util.concurrent.RejectedExecutionException: Task java.util.concurrent.FutureTask@5cefe345 rejected from java.util.concurrent.ThreadPoolExecutor@6fd0da26[Shutting down, pool size = 1, active threads = 1, queued tasks = 6, completed tasks = 34]
displayString: Chris 20
displayString: Chris 21
Alice: value = AAAAAAA
displayString: Chris 22
Alice:java.util.concurrent.RejectedExecutionException: Task java.util.concurrent.FutureTask@6628122c rejected from java.util.concurrent.ThreadPoolExecutor@6fd0da26[Shutting down, pool size = 1, active threads = 1, queued tasks = 2, completed tasks = 38]
displayString: Chris 23
displayString: Chris 24

Process finished with exit code 0

 */