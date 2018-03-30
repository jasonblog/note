package ch12ActiveObject.a12_2a;

import ch12ActiveObject.a12_2a.activeobject.ActiveObject;
import ch12ActiveObject.a12_2a.activeobject.ActiveObjectFactory;

public class Main {
    public static void main(String[] args) {
        ActiveObject activeObject = ActiveObjectFactory.createActiveObject();
        new AddClientThread("Diana", activeObject).start();
    }
}
