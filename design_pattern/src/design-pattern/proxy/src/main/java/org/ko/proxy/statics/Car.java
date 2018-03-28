package org.ko.proxy.statics;

import java.util.Random;

public class Car implements MoveAble {

    //实现开车
    public void move() {
        try {
            Thread.sleep(new Random().nextInt(1000));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
