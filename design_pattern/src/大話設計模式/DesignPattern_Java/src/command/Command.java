package command;

public abstract class Command
{

    private Barbecuer barbecuer;
    public Command(Barbecuer bar)
    {
        this.barbecuer = bar;
    }

    public abstract void excuteCommand();

    public Barbecuer getBarbecuer()
    {
        return barbecuer;
    }

    public void setBarbecuer(Barbecuer barbecuer)
    {
        this.barbecuer = barbecuer;
    }
}
