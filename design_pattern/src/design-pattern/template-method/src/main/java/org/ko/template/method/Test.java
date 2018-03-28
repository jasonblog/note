package org.ko.template.method;

public class Test {

    public static void main(String[] args) {
        AnimalAbstract fish = new Fish();
        AnimalAbstract monkey = new Monkey();
        AnimalAbstract tiger = new Tiger();
        fish.life();
        monkey.life();
        tiger.life();
    }
}
