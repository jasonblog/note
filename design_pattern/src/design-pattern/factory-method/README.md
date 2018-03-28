# 簡單工廠模式

### 1.概念
- 實例化對象，用工廠方法代替new操作
- 工廠模式包括工廠方法模式和抽象工廠模式
- 抽象工廠模式是工廠方法模式的擴展

### 2.意圖
- 定義一個接口來創建對象，但是讓子類來決定那些類需要被實例化
- 工廠方法把實例化的工作推遲到子類中去實現

### 3.什麼情況適合工廠模式
- 有一組類似的對象需要創建
- 在編碼時不能預見需要創建那種類的實例
- 系統需要考慮擴展性，不應依賴於產品類實例如何被創建、組合和表達的細節

### 4.項目中的現狀：
- 在軟件系統中經常面臨著“對象”創建的工作，由於需求的變化，這個對象可能隨之也會發生變化，但它卻擁有比較穩定的接口
- 我們需要提供一種封裝機制來隔離出這個易變對象的變化，從而保持系統中其他依賴，該對象的對象不隨著需求的變化而變化

### 5.基於項目現狀將代碼進行如下設計：
- 儘量鬆耦合，一個對象的依賴對象的變化與本身無關
- 具體產品與客戶端剝離，責任分割

### 6.案例
**1.calculator: 使用簡單工廠模式, 創建控制檯計算器**

![UML類圖](uml/calculator.png)

- 創建Calculator抽象類, 添加計算抽象方法, 添加數值屬性, get set方法
```
    /**
     * 計算器計算基類
     */
    public abstract class OperationAbstract {
    
        private BigDecimal number1;
    
        private BigDecimal number2;
    
        /**
         * 由子類實現運算規則
         * @return
         */
        public abstract BigDecimal calculate();
    }
```

- 創建加減乘除實現類

```
    /**
     * 加法運算
     */
    public class Add extends OperationAbstract{
    
        @Override
        public BigDecimal calculate() {
            return this.getNumber1().add(this.getNumber2());
        }
    }
    
    /**
     * 減法運算
     */
    public class Subtract extends OperationAbstract {
    
        @Override
        public BigDecimal calculate() {
            return this.getNumber1().subtract(this.getNumber2());
        }
    }
    
    /**
     * 乘法運算
     */
    public class Multiply extends OperationAbstract {
    
        @Override
        public BigDecimal calculate() {
            return this.getNumber1().multiply(this.getNumber2());
        }
    }
    
    /**
     * 除法運算
     */
    public class Divide extends OperationAbstract {
    
        @Override
        public BigDecimal calculate() {
            return this.getNumber1().divide(this.getNumber2());
        }
    }
```

- 創建工廠類

```
    /**
     * 計算機靜態工廠類
     */
    public final class OperationFactory {
    
        /**
         * 獲取計算
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
                    throw new RuntimeException("暫不支持");
            }
        }
    
        private OperationFactory () {}
    }
```

- 測試

```
    public static void main(String[] args) {
    
        do {
            //控制檯輸入
            Scanner scanner = new Scanner(System.in);

            //輸入3次
            BigDecimal number1 = scanner.nextBigDecimal();
            String symbol = scanner.next();
            BigDecimal number2 = scanner.nextBigDecimal();

            //通過靜態工廠獲取實例
            OperationAbstract operation = OperationFactory.createOperation(symbol);

            //放入計算數據
            operation.setNumber1(number1);
            operation.setNumber2(number2);

            //獲取返回
            BigDecimal result = operation.calculate();

            System.out.println(result);

        } while (true);
    }
```