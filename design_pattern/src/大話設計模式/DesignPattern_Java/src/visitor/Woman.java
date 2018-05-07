package visitor;

public class Woman extends Person
{

    @Override
    public void accept(Action visitor)
    {
        visitor.getWomanReaction(this);
    }

}
