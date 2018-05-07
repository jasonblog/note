package proxy;

public class Pursuit implements GiveGift
{
    SchoolGirl mm ;

    public Pursuit(SchoolGirl mm)
    {
        this.mm = mm;
    }

    public void giveChocolate()
    {
        System.out.println(mm.getName() + "give you a chocolate");
    }

    public void giveDolls()
    {
        System.out.println(mm.getName() + "give you a doll");
    }

    public void giveFlowers()
    {
        System.out.println(mm.getName() + "give you a Flower");
    }

}
