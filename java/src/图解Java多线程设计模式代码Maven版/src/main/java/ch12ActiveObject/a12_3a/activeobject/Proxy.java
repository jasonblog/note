package ch12ActiveObject.a12_3a.activeobject;

class Proxy implements ActiveObject {
    private final SchedulerThread scheduler;
    private final Servant servant;

    public Proxy(SchedulerThread scheduler, Servant servant) {
        this.scheduler = scheduler;
        this.servant = servant;
    }

    public Result<String> search(String word) {
        FutureResult<String> future = new FutureResult<String>();
        scheduler.invoke(new SearchRequest(servant, future, word));
        return future;
    }
}
