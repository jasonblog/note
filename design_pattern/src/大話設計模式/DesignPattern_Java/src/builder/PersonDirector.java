package builder;

public class PersonDirector
{

    private PersonBuilder pb;

    public PersonDirector(PersonBuilder pb)
    {
        this.pb = pb;
    }

    public void createPerson()
    {
        pb.createHead();
        pb.createBody();
        pb.createArm();
        pb.createLeg();
    }
}
