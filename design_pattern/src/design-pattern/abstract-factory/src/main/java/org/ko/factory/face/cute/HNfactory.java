package org.ko.factory.face.cute;

/**
 * 新年系列工厂
 */
public class HNfactory implements PersonFactory {

    @Override
    public Boy getBoy() {
        return new HNBoy();
    }

    @Override
    public Girl getGirl() {
        return new HNGirl();
    }
}
