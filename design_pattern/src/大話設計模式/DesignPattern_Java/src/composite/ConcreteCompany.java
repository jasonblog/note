package composite;

import java.util.ArrayList;
import java.util.List;

public class ConcreteCompany extends Company
{

    private List<Company> children = new ArrayList<Company>();

    public ConcreteCompany(String name)
    {
        super(name);
    }

    @Override
    public void add(Company c)
    {
        children.add(c);
    }

    @Override
    public void display(int depth)
    {
        System.out.println("-" + this.getName());

        for (Company c : children) {
            for (int i = 0; i < depth; i++) {
                System.out.print("--");
            }

            c.display(depth + 2);
        }
    }

    @Override
    public void lineOfDuty()
    {
        for (Company c : children) {
            c.lineOfDuty();
        }
    }

    @Override
    public void remove(Company c)
    {
        children.remove(c);
    }

}
