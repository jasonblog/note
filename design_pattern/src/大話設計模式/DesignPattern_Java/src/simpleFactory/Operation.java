package simpleFactory;

public class Operation
{
    protected int a;
    protected int b;
    public int getA()
    {
        return a;
    }
    public void setA(int a)
    {
        this.a = a;
    }
    public int getB()
    {
        return b;
    }
    public void setB(int b)
    {
        this.b = b;
    }

    public int getResult()
    {
        return 0;
    }
}
