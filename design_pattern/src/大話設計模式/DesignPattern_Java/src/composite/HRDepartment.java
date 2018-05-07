package composite;

public class HRDepartment extends Company
{

    public HRDepartment(String name)
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
        System.out.println(this.getName() + " recruitment and training!");
    }

    @Override
    public void remove(Company c)
    {

    }

}
