package composite;

public class FinanceDepartment extends Company
{

    public FinanceDepartment(String name)
    {
        super(name);
    }

    @Override
    public void add(Company c)
    {

    }

    @Override
    public void display(int depth)
    {
        System.out.println("-" + this.getName());
    }

    @Override
    public void lineOfDuty()
    {
        System.out.println(this.getName() + "All the things about Finance!");
    }

    @Override
    public void remove(Company c)
    {
    }

}
