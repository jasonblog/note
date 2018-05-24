import java.util.*;

interface Request
{
    void execute();
}

class WorkerThread extends Thread
{
    private Request request;
    private boolean isContinue = true;
    boolean isIdle()
    {
        return request == null;
    }
    void setRequest(Request request)
    {
        synchronized(this) {
            if (isIdle()) {
                this.request = request;
                notify();
            }
        }
    }
    public void run()
    {
        while (isContinue) {
            synchronized(this) {
                try {
                    wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

                request.execute();
                request = null;
            }
        }
    }
    void terminate()
    {
        isContinue = false;
        setRequest(new Request() {
            public void execute() { /* do nothing */ }
        });
    }
}

class WorkerThreadPool
{
    private List<WorkerThread> workerThreads;
    WorkerThreadPool()
    {
        workerThreads = new ArrayList<WorkerThread>();
    }
    synchronized void service(Request request)
    {
        boolean idleNotFound = true;

        for (WorkerThread workerThread : workerThreads) {
            if (workerThread.isIdle()) {
                workerThread.setRequest(request);
                idleNotFound = false;
                break;
            }
        }

        if (idleNotFound) {
            WorkerThread workerThread = createWorkerThread();
            workerThread.setRequest(request);
        }
    }
    synchronized void cleanIdle()
    {
        for (WorkerThread workerThread : workerThreads) {
            if (workerThread.isIdle()) {
                workerThreads.remove(workerThread);
                workerThread.terminate();
            }
        }
    }
    private WorkerThread createWorkerThread()
    {
        WorkerThread workerThread = new WorkerThread();
        workerThread.start();
        workerThreads.add(workerThread);

        try {
            Thread.sleep(1000); // 給點時間進入 Runnable
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        return workerThread;
    }
}

class Service
{
    private WorkerThreadPool pool = new WorkerThreadPool();
    void accept(Request request)
    {
        pool.service(request);
    }
}

// 以下模擬客戶發出請求
class Client implements Runnable
{
    private Service service;
    Client(Service service)
    {
        this.service = service;
    }
    public void run()
    {
        while (true) {
            Request request = new Request() {
                public void execute() {
                    System.out.println("執行客戶請求...XD");

                    try {
                        Thread.sleep((int)(Math.random() * 3000));
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            };
            service.accept(request);

            try {
                Thread.sleep((int)(Math.random() * 3000));
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}

public class Main
{
    public static void main(String[] args)
    {
        Service service = new Service();

        for (int i = 0; i < 5; i++) {
            (new Thread(new Client(service))).start();
        }
    }
}
