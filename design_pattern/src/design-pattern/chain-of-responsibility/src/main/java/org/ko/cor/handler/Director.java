package org.ko.cor.handler;

public class Director extends PriceHandler {

    @Override
    public void processDiscount(float discount) {

        if (discount <= 0.35) {
            System.out.format("%s批准了折扣: %.2f%n", this.getClass().getName(), discount);
        } else {
            successor.processDiscount(discount);
        }
    }

}
