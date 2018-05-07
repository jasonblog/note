package decorator;

public class DecoraterMain
{

    /**
     * @param args
     */
    public static void main(String[] args)
    {
        Person lin = new Person("Lincoln");
        System.out.println("The first show:");

        Sneaker s = new Sneaker();
        Hat h = new Hat();

        s.decorate(lin);
        h.decorate(s);
        h.show();
    }

}
