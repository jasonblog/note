package command;

public class BakeChickenWingCommand extends Command
{

    public BakeChickenWingCommand(Barbecuer bar)
    {
        super(bar);
    }

    @Override
    public void excuteCommand()
    {
        this.getBarbecuer().bakeChickenWing();
    }

}
