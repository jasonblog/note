package org.ko.strategy.promotion;

import java.math.BigDecimal;

public class Test {

    public static void main(String[] args) {
        //初始化上下文
        PromotionContext context = new PromotionContext();

        //测试无促销
        context.newPromotion(1);
        context.setPrice(new BigDecimal("200"));
        BigDecimal price = context.getPrice();
        System.out.println(price);

        //测试打折
        context.newPromotion(2, new BigDecimal("0.8"));
        context.setPrice(new BigDecimal("200"));
        price = context.getPrice();
        System.out.println(price);

        //测试满减
        context.newPromotion(3, new BigDecimal("300"), new BigDecimal("100"));
        context.setPrice(new BigDecimal("200"));
        price = context.getPrice();
        System.out.println(price);
        context.setPrice(new BigDecimal("300"));
        price = context.getPrice();
        System.out.println(price);

//        context.newInstance(4);
    }
}
