package org.ko.decorator.dress;

public class Test {

    public static void main(String[] args) {
        Person person = new Person("K.O");
        BigTrouser trouser = new BigTrouser();
        TShirts tShirts = new TShirts();

        //装饰过程
        trouser.decorator(person);
        tShirts.decorator(trouser);

        tShirts.show();
    }
}
