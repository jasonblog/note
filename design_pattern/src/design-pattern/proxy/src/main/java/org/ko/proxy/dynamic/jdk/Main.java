package org.ko.proxy.dynamic.jdk;

import org.ko.proxy.statics.Car;
import org.ko.proxy.statics.MoveAble;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Proxy;

public class Main {

    public static void main(String[] args) {
        Car car = new Car();

        InvocationHandler handler = new TimeHandler(car);
        Class<?> clazz = car.getClass();
        /**
         * loader 类加载器
         * interfaces 实现接口
         * InvocationHandler
         *
         * ---动态代理实现思路---
         * 实现功能：通过Proxy的newProxyInstance返回代理对象
         * 1.声明一段源码（动态产生代理）
         * 2.编译源码（JDK Complider API）, 产生一个新的类（代理类）
         * 3.将这个类load到内存当中，产生一个新的对象（代理对象）
         * 4.return 代理对象
         */
        MoveAble moveAble = (MoveAble)Proxy.newProxyInstance(clazz.getClassLoader(), clazz.getInterfaces(), handler);
        moveAble.move();
    }
}
