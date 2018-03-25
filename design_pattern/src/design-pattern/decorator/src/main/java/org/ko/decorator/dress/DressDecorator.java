package org.ko.decorator.dress;

/**
 * 装饰类
 */
public class DressDecorator extends Person {

    private Person person;

    /**
     * 添加装饰
     * @param person
     */
    public void decorator (Person person) {
        this.person = person;
    }

    @Override
    public void show () {
        if (this.person != null) {
            this.person.show();
        }
    }

}
