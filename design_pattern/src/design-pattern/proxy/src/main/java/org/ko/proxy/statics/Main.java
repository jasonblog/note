package org.ko.proxy.statics;

public class Main {

    public static void main (String[] args) {
        Car car = new Car();
        car.move();

        //使用聚合--时间上的代理
        CarTimeProxy carTimeProxy = new CarTimeProxy(car);

        //使用日志的处理
        CarLogProxy carLogProxy = new CarLogProxy(carTimeProxy);

        carLogProxy.move();
    }
}
