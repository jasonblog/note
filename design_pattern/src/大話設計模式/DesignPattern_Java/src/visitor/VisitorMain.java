package visitor;

public class VisitorMain
{

    /**
     * @param args
     */
    public static void main(String[] args)
    {
        ObjectStructure os = new ObjectStructure();
        Person man = new Man();
        Person woman = new Woman();

        os.attach(man);
        os.attach(woman);

        Success v = new Success();
        os.display(v);

        Fail f = new Fail();
        os.display(f);
    }

}
