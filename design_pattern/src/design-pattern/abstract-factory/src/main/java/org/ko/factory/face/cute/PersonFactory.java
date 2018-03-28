package org.ko.factory.face.cute;

public interface PersonFactory {

    /**
     * 男孩接口
     * @return
     */
    Boy getBoy();

    /**
     * 女孩接口
     * @return
     */
    Girl getGirl();

}
