package org.ko.factory.calculator;

import java.math.BigDecimal;

/**
 * 除法运算
 */
public class Divide extends OperationAbstract {

    @Override
    public BigDecimal calculate() {
        return this.getNumber1().divide(this.getNumber2());
    }
}
