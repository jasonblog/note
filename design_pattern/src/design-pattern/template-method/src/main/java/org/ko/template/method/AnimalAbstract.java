package org.ko.template.method;

public abstract class AnimalAbstract {

    public void life () {
        eat();
        sport();
        sleep();
    }

    public abstract void eat ();

    public abstract void sport ();

    public abstract void sleep ();
}
