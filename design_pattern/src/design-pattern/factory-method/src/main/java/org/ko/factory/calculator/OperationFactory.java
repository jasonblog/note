package org.ko.factory.calculator;

/**
 * 计算机静态工厂类
 */
public final class OperationFactory {


    /**
     * 获取计算
     * @param symbol
     * @return
     */
    public static OperationAbstract createOperation (String symbol) {
        switch (symbol) {
            case SymbolConstants.ADD:
                return new Add();
            case SymbolConstants.SUBTRACT:
                return new Subtract();
            case SymbolConstants.MULTIPLY:
                return new Multiply();
            case SymbolConstants.DIVIDE:
                return new Divide();
            default:
                throw new RuntimeException("暂不支持");
        }
    }

    /**
     * 常量类
     */
    private final class SymbolConstants {

        private static final String ADD = "+";

        private static final String SUBTRACT = "-";

        private static final String MULTIPLY = "*";

        private static final String DIVIDE = "/";
    }

    private OperationFactory () {}
}
