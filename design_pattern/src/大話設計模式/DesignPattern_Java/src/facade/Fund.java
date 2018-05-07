package facade;

public class Fund
{

    private Stock1 s1;
    private Stock2 s2;
    public Fund()
    {
        s1 = new Stock1();
        s2 = new Stock2();
    }

    public void method1()
    {
        System.out.println("method1: ");
        s1.buy();
        s2.buy();
    }

    public void method2()
    {
        System.out.println("method2: ");
        s2.buy();
        s1.sell();
    }

}
