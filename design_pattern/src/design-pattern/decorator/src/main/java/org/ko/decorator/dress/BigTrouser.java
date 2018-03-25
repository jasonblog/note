package org.ko.decorator.dress;

public class BigTrouser extends DressDecorator {

    @Override
    public void show () {
        System.out.println("裤子");
        super.show();
    }
}
