package org.ko.strategy;

import org.junit.jupiter.api.Test;

import java.math.BigDecimal;

public class PromotionTests {

    @Test
    public void test1 () {
        int i = new BigDecimal("2").compareTo(new BigDecimal("1"));
        System.out.println(i);
    }
}
