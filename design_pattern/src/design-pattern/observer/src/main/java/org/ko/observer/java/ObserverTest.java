package org.ko.observer.java;

public class ObserverTest {

    public static void main(String[] args) {
        //创建一个天气作为一个目标, 也可以说是被观察者
        ConcreteWeatherSubject subject = new ConcreteWeatherSubject();
        //黄明的女朋友作为观察者
        ConcreteObserver girl = new ConcreteObserver();
        girl.setObserverName("黄明的女朋友");
        ConcreteObserver mum = new ConcreteObserver();
        mum.setObserverName("黄明的老妈");

        //注册观察者
        subject.addObserver(girl);
        subject.addObserver(mum);

        //目标更新天气情况了
        subject.setContent("天气晴朗, 气温28度");
    }
}
