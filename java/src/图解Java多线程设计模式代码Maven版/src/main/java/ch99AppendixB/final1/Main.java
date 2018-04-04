package ch99AppendixB.final1;

class Something {
    // 静态字段
    private static Something last = null;
    // final的实例字段
    private final int x;

    // 构造函数
    public Something() {
        // 显式地初始化final字段
        x = 123;
        // 在静态字段中保存正在创建中的实例（this）
        last = this;
    }

    // 通过last显示final字段的值
    public static void print() {
        if (last != null) {
            System.out.println(last.x);
        }
    }
}

public class Main {
    public static void main(String[] args) {
        // 线程A
        new Thread() {
            public void run() {
                new Something();
            }
        }.start();

        // 线程B
        new Thread() {
            public void run() {
                Something.print();
            }
        }.start();
    }
}
