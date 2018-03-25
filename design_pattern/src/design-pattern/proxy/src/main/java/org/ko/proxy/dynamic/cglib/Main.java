package org.ko.proxy.dynamic.cglib;

public class Main {

    public static void main(String[] args) {
        CglibProxy proxy = new CglibProxy();
        Train train = (Train)proxy.getProxy(Train.class);
        train.move();
    }
}
