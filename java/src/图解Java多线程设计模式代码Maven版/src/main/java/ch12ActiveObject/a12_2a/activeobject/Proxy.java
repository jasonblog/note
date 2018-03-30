package ch12ActiveObject.a12_2a.activeobject;

class Proxy implements ActiveObject {
    private final SchedulerThread scheduler;
    private final Servant servant;

    public Proxy(SchedulerThread scheduler, Servant servant) {
        this.scheduler = scheduler;
        this.servant = servant;
    }

    public Result<String> makeString(int count, char fillchar) {
        FutureResult<String> future = new FutureResult<String>();
        scheduler.invoke(new MakeStringRequest(servant, future, count, fillchar));
        return future;
    }

    public void displayString(String string) {
        scheduler.invoke(new DisplayStringRequest(servant, string));
    }

    public Result<String> add(String x, String y) {
        FutureResult<String> future = new FutureResult<String>();
        scheduler.invoke(new AddRequest(servant, future, x, y));
        return future;
    }
}
