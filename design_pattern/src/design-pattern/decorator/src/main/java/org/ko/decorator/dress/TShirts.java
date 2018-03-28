package org.ko.decorator.dress;

public class TShirts extends DressDecorator {

    @Override
    public void show () {
        System.out.println("T恤");
        super.show();
    }
}
