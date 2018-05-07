package factoryMethod;

public class UndergraduateFactory implements IFactory
{

    public LeiFeng createLeiFeng()
    {
        return new Graduate();
    }

}
