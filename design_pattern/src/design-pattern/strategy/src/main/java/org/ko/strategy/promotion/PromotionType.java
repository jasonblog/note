package org.ko.strategy.promotion;

import java.util.Objects;

/**
 * 促销手段样例
 */
public enum PromotionType {

    CashNormal(1, "CashNormal", "原价"),
    CashRebate(2, "CashRebate", "打折"),
    CashReturn(3, "CashReturn", "满减");

    private Integer code;

    private String clazz;

    private String description;

    PromotionType(Integer code, String clazz, String description) {
        this.code = code;
        this.clazz = clazz;
        this.description = description;
    }

    /**
     * 通过编码获取促销手段
     * @param code 促销手段编码
     * @return
     */
    public static String findClazz(Integer code) {
        for (PromotionType type : PromotionType.values()) {
            if (Objects.equals(code, type.code)) {
                return type.clazz;
            }
        }
        return null;
    }
}
