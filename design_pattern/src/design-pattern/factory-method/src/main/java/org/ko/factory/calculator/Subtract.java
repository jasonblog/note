package org.ko.factory.calculator;

import java.math.BigDecimal;

/**
 * 减法运算
 */
public class Subtract extends OperationAbstract {

    @Override
    public BigDecimal calculate() {
        return this.getNumber1().subtract(this.getNumber2());
    }
}
