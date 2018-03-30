package ch00Introduction1.ai1_4;

public class Main {
    public static void main(String[] args) {
        Bank bank = new Bank("A Bad Bank", 1000);   // 创建一个1000日元的银行账户
        new ClientThread(bank).start();
        new ClientThread(bank).start();
    }
}
/*
-----①
可用余额为负数! money = -2000
可用余额为负数! money = -2000
-----②
可用余额为负数! money = 0
可用余额为负数! money = -1000
 */