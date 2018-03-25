package org.ko.strategy.promotion;

import java.math.BigDecimal;

/**
 * 促销策略类
 */
public interface IPromotionStrategy {

    /**
     * 计算逻辑
     */
    BigDecimal promotionAlgorithm();

    /**
     * 存入价钱
     * @param price
     */
    void setPrice(BigDecimal price);
}
