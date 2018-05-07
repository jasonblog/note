package decorator;

public class Person
{

    private String name;
    public Person()
    {
    }
    public Person(String name)
    {
        this.name = name;
    }

    public void show()
    {
        System.out.println(" decorated guy " + this.name);
    }
    public String getName()
    {
        return name;
    }
    public void setName(String name)
    {
        this.name = name;
    }
}
