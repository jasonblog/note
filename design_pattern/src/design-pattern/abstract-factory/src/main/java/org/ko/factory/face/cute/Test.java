package org.ko.factory.face.cute;

public class Test {

    public static void main(String[] args) {
        //抽象工厂
        PersonFactory factory = new MCfactory();
        Girl girl = factory.getGirl();
        girl.drawGirl();
        Boy boy = factory.getBoy();
        boy.drawBoy();

        PersonFactory factory1 = new HNfactory();
        Girl girl1 = factory1.getGirl();
        girl1.drawGirl();
        Boy boy1 = factory1.getBoy();
        boy1.drawBoy();
    }
}
