package org.ko.observer.base;

public class ObserverTest {

    public static void main(String[] args) {

        //1-创建目标
        ConcreteWeatherSubject subject = new ConcreteWeatherSubject();

        //2-创建观察者
        ConcreteObserver girl = new ConcreteObserver();
        girl.setObserverName("女朋友");
        girl.setRemindThing("看电视");

        ConcreteObserver mum = new ConcreteObserver();
        mum.setObserverName("老妈");
        mum.setRemindThing("打麻将");

        //3-注册观察者
        subject.attach(girl);
        subject.attach(mum);

        //4-目标发布天气了
        subject.setWeatherContent("下雨");
        subject.setWeatherContent("下雪");

    }

}
