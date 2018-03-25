package org.ko.factory.calculator;

import java.math.BigDecimal;

/**
 * 加法运算
 */
public class Add extends OperationAbstract{

    @Override
    public BigDecimal calculate() {
        return this.getNumber1().add(this.getNumber2());
    }
}
