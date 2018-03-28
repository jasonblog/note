package org.ko.factory.face.cute;

/**
 * 圣诞系列工厂
 */
public class MCfactory implements PersonFactory {

    @Override
    public Boy getBoy() {
        return new MCBoy();
    }

    @Override
    public Girl getGirl() {
        return new MCGirl();
    }
}
