package composite;

public abstract class Company
{

    private String name;

    public Company(String name)
    {
        this.name = name;
    }

    public String getName()
    {
        return name;
    }

    public void setName(String name)
    {
        this.name = name;
    }

    public abstract void add(Company c);

    public abstract void remove(Company c);

    public abstract void display(int depth);

    public abstract void lineOfDuty();
}
