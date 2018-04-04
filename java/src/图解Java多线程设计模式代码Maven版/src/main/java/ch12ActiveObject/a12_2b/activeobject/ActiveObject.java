package ch12ActiveObject.a12_2b.activeobject;

import java.util.concurrent.Future;

public interface ActiveObject {
    public abstract Future<String> makeString(int count, char fillchar);

    public abstract void displayString(String string);

    public abstract Future<String> add(String x, String y);

    public abstract void shutdown();
}
