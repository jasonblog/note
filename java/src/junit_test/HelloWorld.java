// HelloWorld.java
import java.util.*;
public final class HelloWorld
{
    private String str;
    public static void main(String[] args)
    {
        HelloWorld  hw = new HelloWorld();
        hw.hello();
        System.out.println(hw.str);
    }
    public void hello()
    {
        str = "Hello World!";
    }
    public String getStr()
    {
        return str;
    }
}

