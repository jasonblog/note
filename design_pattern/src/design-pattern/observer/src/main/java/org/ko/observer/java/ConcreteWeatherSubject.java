package org.ko.observer.java;

import java.util.Observable;


/**
 * 天气目标的具体实现类
 */
public class ConcreteWeatherSubject extends Observable {

    //天气的情况
    private String content;

    public String getContent() {
        return content;
    }

    public void setContent(String content) {
        this.content = content;
        //天气情况有了，通知所有观察者
        //通知之前, 在用Java中的ObServer模式时候, 下面这句话不可少
        this.setChanged();
        //进行通知 推模式
        this.notifyObservers(content);

        //拉模式 将setget的引用传递
//        this.notifyObservers();

    }
}
