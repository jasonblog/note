package mediator;

public class Iraq extends Country
{

    public Iraq(UnionNations un)
    {
        super(un);
    }

    public void declare(String message)
    {
        this.getUn().declare(message, this);
    }

    public void getMessage(String message)
    {
        System.out.println("Iraq has get the message: " + message);
    }

}
