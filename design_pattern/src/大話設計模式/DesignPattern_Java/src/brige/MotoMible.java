package brige;

public class MotoMible extends Mobile
{

    public MotoMible(String brand)
    {
        super(brand);
    }

    @Override
    public void run()
    {
        System.out.println("Moto Mobile: ");
        this.getSoft().run();
    }

}
