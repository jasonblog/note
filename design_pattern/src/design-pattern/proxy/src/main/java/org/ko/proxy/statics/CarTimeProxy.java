package org.ko.proxy.statics;

/**
 * 使用聚合的方式实现静态代理
 */
public class CarTimeProxy implements MoveAble {

    //使用聚合的方式来实现代理
    private MoveAble moveAble;

    public CarTimeProxy(MoveAble moveAble) {

        this.moveAble = moveAble;
    }

    public void move() {
        long startTime = System.currentTimeMillis();
        System.out.println("汽车开始行驶...");
        moveAble.move();
        long endTime = System.currentTimeMillis();
        System.out.println("汽车结束行驶...");
        System.out.println("共耗时" + (endTime - startTime) + "毫秒！");
    }
}
