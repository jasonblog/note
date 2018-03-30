package ch02Immutable.a2_5b;

public class Main {
    public static void main(String[] args) {
        // 创建实例
        Point p1 = new Point(0, 0);
        Point p2 = new Point(100, 0);
        Line line = new Line(p1, p2);

        // 显示
        System.out.println("line = " + line);

        // 修改状态
        p1.x = 150;
        p2.x = 150;
        p2.y = 250;

        // 再次显示
        System.out.println("line = " + line);
    }
}
/*
line = [ Line: (0,0)-(100,0) ]
line = [ Line: (0,0)-(100,0) ]
 */