package brige;

public class NokiaMobile extends Mobile
{

    public NokiaMobile(String brand)
    {
        super(brand);
    }

    @Override
    public void run()
    {
        System.out.println("Nokia Mobile: ");
        this.getSoft().run();
    }

}
