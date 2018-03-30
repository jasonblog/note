package ch01SingleThreadedExecution.q1_4;

public final class Point {
    private int x;
    private int y;

    public Point(int x, int y) {
        this.x = x;
        this.y = y;
    }

    public synchronized void move(int dx, int dy) {
        x += dx;
        y += dy;
    }
//（读到这里为止）
}