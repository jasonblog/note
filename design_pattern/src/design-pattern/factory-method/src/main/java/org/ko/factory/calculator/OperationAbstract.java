package org.ko.factory.calculator;

import java.math.BigDecimal;

/**
 * 计算器计算基类
 */
public abstract class OperationAbstract {

    private BigDecimal number1;

    private BigDecimal number2;

    /**
     * 由子类实现运算规则
     * @return
     */
    public abstract BigDecimal calculate();

    public BigDecimal getNumber1() {
        return number1;
    }

    public void setNumber1(BigDecimal number1) {
        this.number1 = number1;
    }

    public BigDecimal getNumber2() {
        return number2;
    }

    public void setNumber2(BigDecimal number2) {
        this.number2 = number2;
    }
}
