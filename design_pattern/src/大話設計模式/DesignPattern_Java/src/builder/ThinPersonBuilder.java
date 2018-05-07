package builder;

public class ThinPersonBuilder extends PersonBuilder
{

    @Override
    public void createArm()
    {
        System.out.println("Create A Thin Arm!");
    }

    @Override
    public void createBody()
    {
        System.out.println("Create A Thin Body!");
    }

    @Override
    public void createHead()
    {
        System.out.println("Create A Thin Head!");
    }

    @Override
    public void createLeg()
    {
        System.out.println("Create A Thin Leg!");

    }

}
