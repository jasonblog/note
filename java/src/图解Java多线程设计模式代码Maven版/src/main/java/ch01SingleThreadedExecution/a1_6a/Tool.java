package ch01SingleThreadedExecution.a1_6a;

public class Tool {
    private final String name;

    public Tool(String name) {
        this.name = name;
    }

    public String toString() {
        return "[ " + name + " ]";
    }
}
