package adapter;

public class Guard extends Player
{

    public Guard(String name)
    {
        this.setName(name);
    }

    public void attrack()
    {
        System.out.println(this.getName() + " attrack");
    }

    public void defence()
    {
        System.out.println(this.getName() + " defence");
    }
}
