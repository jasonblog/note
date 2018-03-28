package org.ko.proxy.dynamic.jdk.simple;


import org.ko.proxy.statics.Car;
import org.ko.proxy.statics.MoveAble;

public class Main {
    public static void main(String[] args) {
        Car car = new Car();
        InvocationHandler h = new TimeHandler(car);
        MoveAble moveable = (MoveAble)Proxy.newProxyInstance(MoveAble.class,h);
        moveable.move();
    }
}
