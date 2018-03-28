package org.ko.proxy.dynamic.jdk.simple;

import java.lang.reflect.Method;

public class TimeHandler implements InvocationHandler {

    private Object object;

    public TimeHandler(Object object) {
        this.object = object;
    }

    @Override
    public void invoke(Object o, Method m) {
        try {
            long startTime = System.currentTimeMillis();
            System.out.println("This car start move...");
            m.invoke(object);
            long endTime = System.currentTimeMillis();
            System.out.println("This car end move...");
            System.out.println("use " + (endTime - startTime) + " ms!");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
