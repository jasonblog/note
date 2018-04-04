package ch02Immutable.a2_2;

public class Main {
    public static void main(String[] args) {
        String s = "BAT";
        String t = s.replace('B', 'C'); // 将'B'替换为'C'
        System.out.println("s = " + s); // 执行replace后的s
        System.out.println("t = " + t); // replace的返回值t
        if (s == t) {
            System.out.println("s == t");
        } else {
            System.out.println("s != t");
        }
    }
}
/*
s = BAT
t = CAT
s != t
 */