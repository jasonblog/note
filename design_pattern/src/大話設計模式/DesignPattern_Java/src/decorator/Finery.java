package decorator;

public class Finery extends Person
{
    private Person component;

    public  void decorate(Person component)
    {
        this.component = component;
    }

    @Override
    public void show()
    {
        if (component != null) {
            component.show();
        }
    }
}
