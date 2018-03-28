package org.ko.template.method;

public class Monkey extends AnimalAbstract {

    @Override
    public void eat() {
        System.out.println("吃香蕉");
    }

    @Override
    public void sport() {
        System.out.println("手舞足蹈");
    }

    @Override
    public void sleep() {
        System.out.println("在树上休息");
    }
}
