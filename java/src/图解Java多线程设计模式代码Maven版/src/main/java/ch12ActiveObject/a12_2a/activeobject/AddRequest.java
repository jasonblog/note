package ch12ActiveObject.a12_2a.activeobject;

class AddRequest extends MethodRequest<String> {
    private final String x;
    private final String y;

    public AddRequest(Servant servant, FutureResult<String> future, String x, String y) {
        super(servant, future);
        this.x = x;
        this.y = y;
    }

    public void execute() {
        Result<String> result = servant.add(x, y);
        future.setResult(result);
    }
}
