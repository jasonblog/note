package org.ko.strategy.promotion;

import java.math.BigDecimal;

/**
 * 打折
 */
public class CashRebate implements IPromotionStrategy {

    private BigDecimal price = BigDecimal.ZERO;

    private BigDecimal rate;

    public CashRebate(BigDecimal rate) {
        this.rate = rate;
    }

    @Override
    public BigDecimal promotionAlgorithm() {
        return this.price.multiply(this.rate);
    }

    public void setPrice(BigDecimal price) {
        this.price = price;
    }
}
