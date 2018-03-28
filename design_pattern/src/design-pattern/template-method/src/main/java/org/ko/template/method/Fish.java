package org.ko.template.method;

public class Fish extends AnimalAbstract {

    @Override
    public void eat() {
        System.out.println("我吃微生物");
    }

    @Override
    public void sport() {
        System.out.println("晃尾巴");
    }

    @Override
    public void sleep() {
        System.out.println("在水中休息");
    }
}
