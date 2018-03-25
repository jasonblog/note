package org.ko.factory.calculator;

import java.math.BigDecimal;

/**
 * 乘法运算
 */
public class Multiply extends OperationAbstract {

    @Override
    public BigDecimal calculate() {
        return this.getNumber1().multiply(this.getNumber2());
    }
}
