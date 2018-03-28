package org.ko.template.method;

public class Tiger extends AnimalAbstract {

    @Override
    public void eat() {
        System.out.println("吃猴子");
    }

    @Override
    public void sport() {
        System.out.println("奔跑");
    }

    @Override
    public void sleep() {
        System.out.println("睡在森林");
    }
}
