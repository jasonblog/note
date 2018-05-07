package factoryMethod;

public class VolunteerFactory implements IFactory
{

    public LeiFeng createLeiFeng()
    {
        return new Volunteer();
    }

}
