package org.ko.decorator.dress;

public class Person {
    /**
     * 姓名
     */
    private String name;

    public Person(String name) {
        this.name = name;
    }

    public Person() {}

    public void show () {
        System.out.print(name);
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }
}
