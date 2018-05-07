package decorator;

public class Sneaker extends Finery
{

    @Override
    public void show()
    {
        System.out.print(" sneaker ");
        super.show();
    }
}
