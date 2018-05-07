package mediator;

public class USA extends Country
{

    public USA(UnionNations un)
    {
        super(un);
    }

    public void declare(String message)
    {
        this.getUn().declare(message, this);
    }

    public void getMessage(String message)
    {
        System.out.println("USA has get the message: " + message);
    }

}
