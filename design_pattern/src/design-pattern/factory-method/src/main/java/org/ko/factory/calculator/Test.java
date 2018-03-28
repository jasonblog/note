package org.ko.factory.calculator;

import java.math.BigDecimal;
import java.util.Scanner;

public class Test {

    public static void main(String[] args) {

        do {
            //控制台输入
            Scanner scanner = new Scanner(System.in);

            //输入3次
            BigDecimal number1 = scanner.nextBigDecimal();
            String symbol = scanner.next();
            BigDecimal number2 = scanner.nextBigDecimal();

            //通过静态工厂获取实例
            OperationAbstract operation = OperationFactory.createOperation(symbol);

            //放入计算数据
            operation.setNumber1(number1);
            operation.setNumber2(number2);

            //获取返回
            BigDecimal result = operation.calculate();

            System.out.println(result);

        } while (true);
    }
}
