package brige;

public abstract class Mobile
{

    private MobileSoft soft;
    private String brand;

    public Mobile(String brand)
    {
        this.brand = brand;
    }

    public MobileSoft getSoft()
    {
        return soft;
    }

    public void setSoft(MobileSoft soft)
    {
        this.soft = soft;
    }

    public String getBrand()
    {
        return brand;
    }

    public void setBrand(String brand)
    {
        this.brand = brand;
    }

    public abstract void run();
}
