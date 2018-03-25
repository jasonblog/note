package org.ko.strategy.promotion;

import java.math.BigDecimal;

/**
 * 满减
 */
public class CashReturn implements IPromotionStrategy {

    private BigDecimal price;

    private BigDecimal minPrice;

    private BigDecimal subPrice;

    public CashReturn(BigDecimal minPrice, BigDecimal subPrice) {
        this.minPrice = minPrice;
        this.subPrice = subPrice;
    }

    @Override
    public BigDecimal promotionAlgorithm() {
        if (price.compareTo(minPrice) > -1) {
            return this.price.subtract(this.subPrice);
        }
        return this.price;
    }

    public void setPrice(BigDecimal price) {
        this.price = price;
    }
}
