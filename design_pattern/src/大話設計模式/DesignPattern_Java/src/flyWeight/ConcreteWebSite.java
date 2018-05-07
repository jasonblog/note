package flyWeight;

public class ConcreteWebSite extends WebSite
{
    private String name;

    public ConcreteWebSite(String name)
    {
        this.name = name;
    }

    @Override
    public void use(User u)
    {
        System.out.println("the type of Website: " + name + " user: " + u.getName());
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
