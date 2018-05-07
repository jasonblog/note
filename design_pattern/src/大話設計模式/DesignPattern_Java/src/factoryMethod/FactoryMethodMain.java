package factoryMethod;

public class FactoryMethodMain
{

    /**
     * @lincoln liu
     */
    public static void main(String[] args)
    {
        IFactory factory = new UndergraduateFactory();
        Graduate student = (Graduate) factory.createLeiFeng();
        student.buyRice();
        student.Wash();

        IFactory VFactory = new VolunteerFactory();
        Volunteer volunteer = (Volunteer) VFactory.createLeiFeng();
        volunteer.buyRice();
        volunteer.sweep();

    }

}
