package org.ko.strategy.promotion;

import java.math.BigDecimal;

/**
 * 原价
 */
public class CashNormal implements IPromotionStrategy{

    private BigDecimal price;

    @Override
    public BigDecimal promotionAlgorithm() {
        return this.price;
    }

    public void setPrice(BigDecimal price) {
        this.price = price;
    }
}
