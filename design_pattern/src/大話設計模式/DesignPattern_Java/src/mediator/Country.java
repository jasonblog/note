package mediator;

public abstract class Country
{
    private UnionNations un;

    public Country(UnionNations un)
    {
        this.un = un;
    }

    public UnionNations getUn()
    {
        return un;
    }

    public void setUn(UnionNations un)
    {
        this.un = un;
    }

}
